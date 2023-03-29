/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PlsIf.c : Command Pulse Interface															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function : Command Pulse Interface																*/
/*																									*/
/*			1)Initialize Pulse Interface Module														*/
/*			2)Pulse Interface ScanA Handler															*/
/*			3)Pulse Interface ScanB Handler 														*/
/*			4)Pulse Interface Parameter Calculation 												*/
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
/*																									*/
/*		Rev.1.00 : 2013.8.13	K.Ozaki																*/
/*		Rev.1.10 : 2013.10.1	K.Ozaki			<S00C>												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxPlsScanAService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
void	KpxPlsScanBService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
//void	KpxPlsScanCService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
void	KpxInitPls( void *pAxRsc ); 		/* Initialize Command Pulse Interface 					*/
/*--------------------------------------------------------------------------------------------------*/
LONG	KpiPcalPerClrMode( PLSIFV *pPls, LONG data );	/* Initial Parameter calculation			*/
void	KpiPcalEdgeClrDisTime( 							/* Perr Clear Edge Clear Disable Time		*/
			PLSIFV *pPls, 
			USHORT time, 
			ULONG cycle );	
/*--------------------------------------------------------------------------------------------------*/
void	KpiChkPerClearSignal( PLSIFV *pPls ); 	/* Perr Clear Latch Signal Detect Service			*/
/*--------------------------------------------------------------------------------------------------*/
void	KpiPlsPerClrLatchPosition( 				/* Input PERR Clear Latched Position 				*/
			PLSIFV *pPls, 						/* 			 										*/
			LONG PosFbki,						/* Feedback Position 								*/
			LONG MotAngle,						/* Motor Angle  									*/
			LONG *pClrPos );					/* Latched Position 								*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Functions : Command Pulse Interface													*/
/*--------------------------------------------------------------------------------------------------*/
static void LpxPlsServiceA( struct _PLSIFV *this );	/* ScanA command pulse handler					*/
static void LpxPlsServiceB( struct _PLSIFV *this );	/* ScanB command pulse handler					*/
/*--------------------------------------------------------------------------------------------------*/
static void	LpxSetCmdFilSelPort( struct _PLSIFV *this ); /* set command pulse filter selection port	*/

/****************************************************************************************************/
/*																									*/
/*		Variables																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define _UDL_REG_ADDR_  (ULONG)0xC0000000
#define _UDL_REG_ADDR_OFFSET_  (ULONG)0x00001000
#define AREG_LODSET_SETV			0x02C0
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define ASIC_WR( reg, value ) *(pPls->##reg) = (value)
#define ASIC_RD( reg, value ) (value) = (SHORT)*(pPls->##reg)

/*--------------------------------------------------------------------------------------------------*/
#define KPI_LATCH_PULSECNTR( ch ) { 																\
ASIC_HW_IF_STR *pAsic = (ASIC_HW_IF_STR *)(_UDL_REG_ADDR_ + (_UDL_REG_ADDR_OFFSET_ * ch)); 			\
		pAsic->AREG_LODSET = AREG_LODSET_SETV;														\
}
/*--------------------------------------------------------------------------------------------------*/
void	KpxLatchPulsCntr0( void ) { KPI_LATCH_PULSECNTR( 0 ); }	/* Latch Pulse Counter(Ch:0) */
void	KpxLatchPulsCntr1( void ) { KPI_LATCH_PULSECNTR( 1 ); }	/* Latch Pulse Counter(Ch:1) */

/*--------------------------------------------------------------------------------------------------*/
#define KPI_CHK_LATCHTIMER_CCU( ch )						/* Check Timer Latch 3u					*/\
LONG	lwk0,lwk1;											/*										*/\
ASIC_HW_IF_STR *pAsic = (ASIC_HW_IF_STR *)(_UDL_REG_ADDR_ + (_UDL_REG_ADDR_OFFSET_ * ch)); 			\
		lwk0 = (pAsic->AREG_STIMSTAT); 						/* Read Latch Status 					*/\
		lwk0 = (( lwk0 & (1<<11) ) ? (TRUE) : FALSE);		/* lwk == TRUE ? LatchComplete			*/\
		lwk1 = (pAsic->AREG_STIMLT3);						/* read Latch Timer						*/\
		lwk1 = ((lwk0) ? (lwk1) : 0 );						/* set Latch Value						*/\

#define KPI_CHK_LATCHTIMER_CCD( ch )						/* Check Timer Latch 3d					*/\
LONG	lwk0,lwk1;											/*										*/\
ASIC_HW_IF_STR *pAsic = (ASIC_HW_IF_STR *)(_UDL_REG_ADDR_ + (_UDL_REG_ADDR_OFFSET_ * ch)); 			\
		lwk0 = (pAsic->AREG_STIMSTAT); 						/* Read Latch Status 					*/\
		lwk0 = (( lwk0 & (1<<15) ) ? (TRUE) : FALSE);		/* lwk == TRUE ? LatchComplete			*/\
		lwk1 = (pAsic->AREG_STIMLT7);						/* read Latch Timer						*/\
		lwk1 = ((lwk0) ? (lwk1) : 0 );						/* set Latch Value						*/\
/*--------------------------------------------------------------------------------------------------*/
LONG	KpxChkClrLatchTimer0u( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCU( 0 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
LONG	KpxChkClrLatchTimer0d( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCD( 0 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
LONG	KpxChkClrLatchTimer1u( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCU( 1 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
LONG	KpxChkClrLatchTimer1d( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCD( 1 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Initialize Virtual Serial Message Interface													*/
/*																									*/
/****************************************************************************************************/
void	KpxInitPls( void *pAxis )
{
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)pAxis;
PLSIFV *pPls = pAxRsc->pPlsIfV;
ASIC_HW_IF_STR *pAsic = pAxRsc->SvAsicRegs->AsicHwReg;
LONG ch = pAxRsc->AxisNo;
/*--------------------------------------------------------------------------------------------------*/
/*	JL086��PulseIF�������́ASHal_InitAsicPulseIF( )�ɂĎ��s											*/
/*--------------------------------------------------------------------------------------------------*/
	;
/*--------------------------------------------------------------------------------------------------*/
/* 	Reset Memory 																					*/
/*--------------------------------------------------------------------------------------------------*/
	/*
	 * �� �����p�����[�^�v�Z�ŏ�����������̂����A�c���Ă���
	 *
	 *
	 */
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( (ULONG *)(&(pPls->f)), (sizeof( PLSIFV )-sizeof( PLSIFP ))/4 );	

/*--------------------------------------------------------------------------------------------------*/
/*		Read Initial Pulse Command Counter															*/
/*--------------------------------------------------------------------------------------------------*/
	KPI_LATCH_PULSECNTR( ch );							/* Latch ASIC  Pulse Counter				*/
	pPls->PcmdHwCntr  = KPI_READ_PCMDCNTR( );			/* Read  Pulse Command Counter				*/
	pPls->dPcmdHwCntr = 0;								/* Reset Delta Pulse Command				*/	
/*--------------------------------------------------------------------------------------------------*/
	pPls->CMDLDL = &(pAsic->AREG_CMDLDL);				/* pointer to the command pulse load latch  */
	pPls->POCSET = &(pAsic->AREG_POCSET);				/* pointer to the command pulse data value  */
	pPls->CMDCLML = &(pAsic->AREG_CMDCLML);				/* pointer to the command pulse latch value */
/*--------------------------------------------------------------------------------------------------*/
	pPls->TIM = &(pAsic->AREG_TIM);						/* @@										*/
	pPls->SFBLT = &(pAsic->AREG_SFBLT4);				/* @@										*/
	pPls->MposSftR = pAxRsc->MencV->MposSftR;			/* @@										*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�b���SGDV�Ɠ����␳�l�ɂ���																*/
	/*----------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrMecha )
	{
		pAsic->AREG_ENCOFS = (ULONG)(-PFBKCOMPDELAY); /* set ENCOFS with Machine Base Timer Offset */
	}
	else
	{
		pAsic->AREG_ENCOFS = (ULONG)PFBKPOUTDELAY;  /* set ENCOFS with Controler Base Timer Offset */
	}
	/*----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*	�G�b�W���o���\�b�h�̐ݒ�																		*/
/*	����Ƀp�����[�^�v�Z���邱��																	*/
/*	���G�b�W���o�_����UDL�̘_���ݒ�Őݒ肷��B														*/
/*--------------------------------------------------------------------------------------------------*/
	pPls->CHK_LATCHTIMER = ((ch == 0 ) ? KpxChkClrLatchTimer0u : KpxChkClrLatchTimer1u);
	pPls->CHK_LATCHTIMER( &(pPls->PerClrEdgeTime) );	/* Clear Latch Status (�O�̂���)			*/

/*--------------------------------------------------------------------------------------------------*/
	if( ch == 0 )
	{
		pPls->LATCH_PULSCNTR = KpxLatchPulsCntr0;		/* pointer to the Function to Latch Puls	*/
	}
	else
	{
		pPls->LATCH_PULSCNTR = KpxLatchPulsCntr1;		/* pointer to the Function to Latch Puls	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	LpxSetCmdFilSelPort( pPls );						/* set hardware command pulse filter port	*/
/*--------------------------------------------------------------------------------------------------*/
	if( pAxRsc->UniPrms->Bprm->RvsDir )
	{
		pPls->RvsDir = (-1);							/* Reverse Mode */
	}
	else
	{
		pPls->RvsDir = (1);								/* Normal Mode */
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Command Pulse Manager( ScanA Cycle )														*/
/*																									*/
/****************************************************************************************************/
void	KpxPlsScanAService( PLSIFV *pPls )
{
	if(PLS_CH0_USE == 1){ LpxPlsServiceA( pPls ); }
//	if(PLS_CH1_USE == 1){ LpxPlsServiceA( &(LPlsIfV[1]) ); }
}

/****************************************************************************************************/
/*																									*/
/*		Command Pulse Manager( ScanB Cycle )														*/
/*																									*/
/****************************************************************************************************/
void	KpxPlsScanBService( PLSIFV *pPls )
{
	if(PLS_CH0_USE == 1){ LpxPlsServiceB( pPls ); }
//	if(PLS_CH1_USE == 1){ LpxPlsServiceB( &(LPlsIfV[1]) ); }
}

/****************************************************************************************************/
/*																									*/
/*		ScanA command pulse handler																	*/
/*																									*/
/****************************************************************************************************/
void	LpxPlsServiceA( struct _PLSIFV *pPls )	
{
/*--------------------------------------------------------------------------------------------------*/
/*		Read Latch Timer Current Value																*/
/*--------------------------------------------------------------------------------------------------*/
		ASIC_RD( TIM, pPls->LatchBaseTimeA );
/*--------------------------------------------------------------------------------------------------*/
/* 		Latch Asic Pulse Counter																	*/ 
/*--------------------------------------------------------------------------------------------------*/
		pPls->LATCH_PULSCNTR( );
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		ScanB command pulse handler																	*/
/*																									*/
/****************************************************************************************************/
void	LpxPlsServiceB( struct _PLSIFV *pPls )	
{
LONG	lwk0;
LONG	lwk1;
/*--------------------------------------------------------------------------------------------------*/
/*		Read Pulse Command Counter																	*/
/*--------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	�O��w�߈ʒu�̍X�V																		*/
		/*------------------------------------------------------------------------------------------*/
		if( pPls->f.PerClrEdge )							/* �΍��G�b�W�N���A����					*/
		{
			lwk0 = pPls->ClrPcmdPastCntr;					/* �O��w�߈ʒu��␳�ʒu�ɕύX			*/
			lwk1 = pPls->PcmdHwCntr;
		}
		else
		{
			lwk0 = pPls->PcmdHwCntr;						/* �O��w�߈ʒu���X�V					*/
			lwk1 = pPls->PcmdHwCntr;
		}
		/*------------------------------------------------------------------------------------------*/
		/* Read and Renewal Pulse Command Counter Info. 											*/
		/*------------------------------------------------------------------------------------------*/
		pPls->PcmdHwCntr = KPI_READ_PCMDCNTR( );			/* Read Pulse Command Counter			*/
		pPls->dPcmdHwCntr = (SHORT)(pPls->PcmdHwCntr - lwk0);
		pPls->dPcmdInputHwCntr = (SHORT)(pPls->PcmdHwCntr - lwk1);
/*--------------------------------------------------------------------------------------------------*/
		pPls->PcmdRingCntr += pPls->dPcmdHwCntr;			/* Pulse Counter for Network I/F		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Pulse Command Input Signal																	*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_READ_PCMD_STATUS( lwk0 );						/* Read Pcmd Status						*/
		pPls->f.TrcPcmdInon = KPX_CHECK_PCMD_CADAT( lwk0 );	/* Trace : Pulse Command Input On		*/
		pPls->f.TrcPcmdSign = KPX_CHECK_PCMD_CBDAT( lwk0 );	/* Trace : Pulse Command Input Sign		*/
		pPls->f.PerClrSignal = KPX_CHECK_PCMD_CCDAT( lwk0 );/* Position Error Clear Signal			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Read Latch Base Timer		 																*/
/*--------------------------------------------------------------------------------------------------*/
		pPls->LatchBaseTimeB = pPls->LatchBaseTimeA;

/*--------------------------------------------------------------------------------------------------*/
/*		Check CLR Signal Edge, Read Latch Timer		 												*/
/*--------------------------------------------------------------------------------------------------*/
		pPls->f.EdgeDetected = pPls->CHK_LATCHTIMER( &(pPls->PerClrEdgeTime) );

/*--------------------------------------------------------------------------------------------------*/
/*		Check Perr Clear Signal (LTM)		 														*/
/*--------------------------------------------------------------------------------------------------*/
		KpiChkPerClearSignal( pPls );						/* check perr clear signal edge			*/

/*--------------------------------------------------------------------------------------------------*/
/*		@@ Next Command Position																	*/
/*--------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	�ʒu�w�ߕ␳���Z		���X�L�����̑O��w�߈ʒu -> BoutK.ClrPcmdPastCntr				*/
		/*------------------------------------------------------------------------------------------*/
		/*		���w��ׯ������^�������̏����iSGDS�Ɠ��d�l�ɂ���j									*/
		/*			�E�����ˎ����݂̑O��w�߈ʒu���G�b�W���́i���b�`�j���̎w�߈ʒu�Ƃ���			*/
		/*			�E�����ˎ����݂̑O��w�߈ʒu�����񽷬݂̎w�߈ʒu�Ƃ���							*/
		/*------------------------------------------------------------------------------------------*/
		if( pPls->f.ClrEnableFlg )							/*	�w�߃��b�`������					*/
		{
			pPls->ClrPcmdPastCntr = pPls->ClrLatchRefPos;	/* ���b�`�w�߈ʒu��O��l�Ƃ���			*/
		}
		else												/* �w�߃��b�`������						*/
		{
			pPls->ClrPcmdPastCntr = pPls->PcmdHwCntr;		/* �O��w�߈ʒu�͍���w�߈ʒu�Ƃ���		*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		�ʒu�΍��N���A�M���Ď��T�[�r�X																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		<V904>  �ʒu�΍��N���A�̃`���^�����O�h�~�Ƃ��āA�ʒu�΍��N���A�i�G�b�W�N���A�j�����s������	*/
/*				�ʒu�΍��N���A�M�����͍ŏ����ԊԊu(Pn240)�o�ߒ��̓N���A�M�����������b�`��Ԃ̃N��	*/
/*				�A���s���悤�ɏC�������B															*/
/*				�܂��A�o�ߒ��ȊO�ł��A�uASIC�̃��b�`�����v&&�uCPU�F�������v���Q��A�������ꍇ�́A	*/
/*				ASIC�̃��b�`��������������悤�ɏC�������B����́AASIC�̂݃��b�`������Ԃ��c���	*/
/*				����CPU�����b�`�M����F�������Ƃ���FB�J�E���^�ƈȑO�̃��b�`�J�E���^�ňʒu�w�߂�		*/
/*				��������\������댯�������邽�߂ł���B											*/
/****************************************************************************************************/
void	KpiChkPerClearSignal( PLSIFV *pPls )
{
LONG	lwk;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��N���A�M���������Ԍo�߁H															*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPls->EdgeClr_DisableCnt == 0 )
	{
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��N���A�M���������Ԍo�ߌ�															*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�ʒu�΍��N���A�M���L��																	*/
	/*----------------------------------------------------------------------------------------------*/
		if( pPls->f.EdgeDetected  )							/* �����ݗv���`�F�b�N					*/
		{
			KPX_READ_PCMD_STATUS( lwk );					/* AREG_POCSET�̓Ǎ���					*/
			if( lwk & 0x8000 )								/* �w�߃J�E���^�̃��b�`����				*/
			{
				pPls->f.ClrEnableFlg = TRUE;				/* �w�߃J�E���^�̃��b�`����				*/
				pPls->ClrLatchRefPos = (SHORT)*(pPls->CMDCLML);/* �G�b�W���͎��̎w�߃J�E���^�Ǎ���	*/
			}
			else
			{
				pPls->f.ClrEnableFlg = FALSE;				/* �w�߃J�E���^�̃��b�`����				*/
				pPls->ClrLatchRefPos = 0;					/* ���̏ꍇ���g�p�F�O�ɂ��Ă���			*/
			}
			pPls->f.PerClrEdge = TRUE;						/* �ʒu�΍��N���A�G�b�W�M���ݒ�			*/
			ASIC_WR( POCSET, (lwk & 0xFEFF));				/* �w�߃J�E���^�̃��b�`����				*/
			KlibWaitus( 1 );								/* ���b�菈�u	�҂����Ԃ̊m�F�v		*/
			ASIC_WR( POCSET, (lwk | 0x0100));				/* �w�߃J�E���^�̃��b�`����				*/
			pPls->EdgeClr_DisableCnt = pPls->P.EdgeClr_DisableTime;		/* �o�ߎ��ԃZ�b�g			*/
			pPls->f.EdgeClrUnmatch = FALSE;					/* �s��v��Ԃ̃N���A					*/
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		�ʒu�΍��N���A�M������																	*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			pPls->f.PerClrEdge = FALSE;						/* �ʒu�΍��N���A�G�b�W�M���N���A		*/
			pPls->f.ClrEnableFlg = FALSE;
			pPls->ClrLatchRefPos = 0;						/* ���̏ꍇ���g�p�F�O�ɂ��Ă���			*/

			KPX_READ_PCMD_STATUS( lwk );					/* AREG_POCSET�̓Ǎ���					*/
			if( lwk & 0x8000 )								/* �w�߃J�E���^�̃��b�`����?			*/
			{
				if( pPls->f.EdgeClrUnmatch == FALSE )		/* �O��s��v��Ԃł͂Ȃ��H				*/
				{
					pPls->f.EdgeClrUnmatch = TRUE;			/* �s��v��Ԃ̃Z�b�g					*/
				}
				else
				{
					ASIC_WR( POCSET, (lwk & 0xFEFF));		/* �w�߃J�E���^�̃��b�`����				*/
					KlibWaitus( 1 );						/* ���b�菈�u	�҂����Ԃ̊m�F�v		*/
					ASIC_WR( POCSET, (lwk | 0x0100));		/* �w�߃J�E���^�̃��b�`����				*/
					pPls->f.EdgeClrUnmatch = FALSE;			/* �s��v��Ԃ̃N���A					*/
				}
			}
			else
			{
				pPls->f.EdgeClrUnmatch = FALSE;				/* �s��v��Ԃ̃N���A					*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��N���A�M���������Ԍo�ߑO�i�������j													*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		pPls->f.PerClrEdge = FALSE;							/* �ʒu�΍��N���A�G�b�W�M���N���A		*/
		pPls->f.ClrEnableFlg = FALSE;
		pPls->ClrLatchRefPos = 0;							/* ���̏ꍇ���g�p�F�O�ɂ��Ă���			*/

		KPX_READ_PCMD_STATUS( lwk );						/* AREG_POCSET�̓Ǎ���					*/
		if( lwk & 0x8000 )									/* �w�߃J�E���^�̃��b�`����?			*/
		{
			ASIC_WR( POCSET, (lwk & 0xFEFF));				/* �w�߃J�E���^�̃��b�`����				*/
			KlibWaitus( 1 );								/* ���b�菈�u	�҂����Ԃ̊m�F�v		*/
			ASIC_WR( POCSET, (lwk | 0x0100));				/* �w�߃J�E���^�̃��b�`����				*/
		}

		pPls->f.EdgeClrUnmatch = FALSE;						/* �M���F���s��v��Ԃ̃N���A			*/
		pPls->EdgeClr_DisableCnt--;							/* �o�ߎ��Ԃ̃f�N�������g				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*	�ʒu�΍��N���A�G�b�W�M���ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrEdgeMode == FALSE ) { pPls->f.PerClrEdge = FALSE;	}	
/*--------------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		Initial Parameter calculation (Pn200)														*/
/*																									*/
/****************************************************************************************************/
LONG	KpiPcalPerClrMode( PLSIFV *pPls, LONG data )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.1 : �N���A�M���`�ԑI���X�C�b�`														*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( data >> 4 ) & 0x0F )
	{
		case 0x00:	pPls->P.PerClrSigSel = 0x00; break;	/* �M���̂g�����x���ŕ΍��N���A				*/
		case 0x01:  pPls->P.PerClrSigSel = 0x01; break;	/* �M���̗����オ��ŕ΍��N���A				*/
		case 0x02:  pPls->P.PerClrSigSel = 0x02; break;	/* �M���̂k�����x���ŕ΍��N���A				*/
		case 0x03:  pPls->P.PerClrSigSel = 0x03; break;	/* �M���̗���������ŕ΍��N���A				*/
		default : return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.2 : �N���A����I��ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( data >> 8 ) & 0x0F )
	{
		case 0x00:	pPls->P.PerClrMode = 0x00; break;	/* �x�[�X�u���b�N���{CLR�M���ŁA�΍��N���A	*/
		case 0x01:	pPls->P.PerClrMode = 0x01; break;	/* CLR�M���ł̂ݕ΍��N���A					*/
		case 0x02:	pPls->P.PerClrMode = 0x02; break;	/* �A���[���������{CLR�M���ŁA�΍��N���A	*/
		case 0x03:	pPls->P.PerClrMode = 0x03; break;	/* �΍��N���A���Ȃ�(CLR�M������)			*/
		default : return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍����x���N���A�I��ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrSigSel == 0x00 )	 	{ pPls->P.PerClrLvlSel = 0x01;}	/* High Level			*/
	else if( pPls->P.PerClrSigSel == 0x02 )	{ pPls->P.PerClrLvlSel = 0x02;}	/* Low  Level			*/
	else                                 	{ pPls->P.PerClrLvlSel = 0x00;}	/* No   Level			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��G�b�W�N���A�I��ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ((pPls->P.PerClrSigSel == 0x01)||(pPls->P.PerClrSigSel == 0x03))&&(pPls->P.PerClrMode != 0x03) )
	{
		pPls->P.PerClrEdgeMode = TRUE;					/* CLR�M���̃G�b�W�ŁA�΍��N���A����		*/
	}
	else
	{
		pPls->P.PerClrEdgeMode = FALSE;					/* CLR�M���̃G�b�W�ŁA�΍��N���A���Ȃ�		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.3 : �t�B���^�I��ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (data >> 12) & 0x0F )
	{
	case 0x00: pPls->P.CmdOpenCollector = FALSE;   break; /* ���C���h���C�o�M���p�w�߃t�B���^�P	*/
	case 0x01: pPls->P.CmdOpenCollector = TRUE;    break; /* �I�[�v���R���N�^�M���p�w�߃t�B���^	*/
	case 0x02: pPls->P.CmdOpenCollector = FALSE;   break; /* ���C���h���C�o�M���p�w�߃t�B���^�Q	*/
	default :  return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
/*	FB�ʒu�␳���Z�t���O�F�b���SGDV�Ɠ����ɂ���B													*/
/*--------------------------------------------------------------------------------------------------*/
	pPls->P.PerClrMecha = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	return (OK);
}


/****************************************************************************************************/
/*																									*/
/*		Pn240: �ʒu�΍��N���A�M�����͍ŏ����ԊԊu�v�Z	<V904>										*/
/*			�ʒu�΍��N���A�i�G�b�W�N���A�j�̗L���G�b�W�F�������莞�ԃN���A�M���𖳎�����			*/
/*			���Ԃ��v�Z����B																		*/
/*																									*/
/*		@@ parameters																				*/
/*		1) ch : Command Pulse Interface Channel Number												*/
/*		2) time : Perr Clear Disable Time [ms]														*/
/*		3) cycle : Scan cycle[us]																	*/
/*																									*/
/****************************************************************************************************/
void	KpiPcalEdgeClrDisTime( PLSIFV *pPls, USHORT time, ULONG cycle )	/* �΍��N���A�������Ԍv�Z	*/
{
		pPls->P.EdgeClr_DisableTime = (LONG)( (ULONG)time * 1000 / cycle );
		return;
}

/****************************************************************************************************/
/*																									*/
/*		�΍��N���A�M�����b�`�ʒu�v�Z																*/
/* 		Input PERR Clear Latched Position 															*/
/*																									*/
/****************************************************************************************************/
#if 0
void	KpiPlsPerClrLatchPosition( 				/* Input PERR Clear Latched Position 				*/
			PLSIFV *pPls, 						/* Channel No. 										*/
			LONG PosFbki,						/* Feedback Position 								*/
			LONG dPosFbki,						/* Delta Position 									*/
			LONG *pClrPos )						/* Latched Position 								*/
{
LONG dFbPosCmp;
	/*----------------------------------------------------------------------------------------------*/
	/*	�e�a�ʒu�␳���Z		�G�b�W���͎��̂e�a�ʒu�����l -> dFbPosCmp							*/
	/*----------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrMecha )							/* �@�B�ʒu��΍��N���A					*/
	{
		dFbPosCmp = KPI_PFBKCOMPEN( dPosFbki, pPls->PerClrEdgeTime );
	}
	else												/* ��ʃR���g���[����΍��N���A			*/
	{
		dFbPosCmp = KPI_PFBKCOMPENHC( dPosFbki, pPls->PerClrEdgeTime );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	@@ �e�a�ʒu�v�Z			�G�b�W���͎��̂e�a�ʒu -> BoutK.ClrFbPos							*/
	/*----------------------------------------------------------------------------------------------*/
	if ( pPls->f.EdgeDetected )
	{
		*pClrPos = PosFbki - dFbPosCmp;
//		pPls->ClrFbPosSft = PosFbki - dFbPosCmp; 		/* @@  ��ŏ���								*/
	}
	/*----------------------------------------------------------------------------------------------*/
	return;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		�΍��N���A�M�����b�`�ʒu�v�Z																*/
/* 		(UDL�̃��b�`�@�\���g�p)																		*/
/*		TODO: ���j�A�A�t���N																		*/
/*																									*/
/****************************************************************************************************/
void	KpiPlsPerClrLatchPosition( 				/* Input PERR Clear Latched Position 				*/
			PLSIFV *pPls, 						/* Pulse Interface 									*/
			LONG PosFbki,						/* Feedback Position [pulse] 						*/
			LONG MotAngle,						/* Motor Angle [pulse] 								*/
			LONG *pClrPos )						/* Latched Position [pulse]							*/
{
LONG dFbPosCmp;
LONG LatchPos = (((LONG)*(pPls->SFBLT)) << (pPls->MposSftR));
	/*----------------------------------------------------------------------------------------------*/
	/*	�e�a�ʒu�␳���Z		�G�b�W���͎��̂e�a�ʒu�����l -> dFbPosCmp							*/
	/*----------------------------------------------------------------------------------------------*/
	dFbPosCmp = (pPls->RvsDir) * ((MotAngle - LatchPos) >> (pPls->MposSftR));
	/*----------------------------------------------------------------------------------------------*/
	/*	@@ �e�a�ʒu�v�Z			�G�b�W���͎��̂e�a�ʒu -> BoutK.ClrFbPos							*/
	/*----------------------------------------------------------------------------------------------*/
	if ( pPls->f.EdgeDetected )
	{
		*pClrPos = PosFbki - dFbPosCmp;
		pPls->ClrFbPos = PosFbki - dFbPosCmp; /* @@ ���j�^�p�F��ŏ��� */ 
	}
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�p���X�w�߃t�B���^�F�t�B���^�I���|�[�g�̐ݒ�												*/
/*																									*/
/****************************************************************************************************/
void	LpxSetCmdFilSelPort( PLSIFV *pPls )
{
/*--------------------------------------------------------------------------------------------------*/
/*	Iprm.f.CmdOpenCollector == TRUE (Pn200.3=1) ==> �I�[�v���R���N�^�M���p�w�߃t�B���^				*/
/*	Iprm.f.CmdOpenCollector == FALSE(Pn200.3=0) ==> ���C���h���C�o�M���p�w�߃t�B���^				*/
/*--------------------------------------------------------------------------------------------------*/
	if(pPls->P.CmdOpenCollector == TRUE)
	{
		KPI_PCMDFIL_ON( );		/* FILSEL ==> High Level	*/
	}
	else
	{
		KPI_PCMDFIL_OFF( );		/* FILSEL ==> Lo Level	*/
	}
}

/***************************************** end of file **********************************************/
