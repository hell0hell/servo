/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Anlg.c : Analog Command Input Module  														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Check the PcmdRingCntr 																	*/
/*		2. Set the Interpolate Motion																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.23  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"
#include "Parameter.h"		/* @@Chk Servo Parameter */

/****************************************************************************************************/
/*																									*/
/*		Anlg Definitions																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Macro Definitions																			*/
/*																									*/
/****************************************************************************************************/
#define OBJ_WRITE( obj, idx, data, hmsg ) ((obj)->Write( (obj), (idx), (data), hmsg ))
#define OBJ_READ( obj, idx, pdata, hmsg ) ((obj)->Read( (COBJECT *)(obj), (idx), (pdata), MCHK_REG_READ, hmsg ))	/* <S073> */
#define OBJ_SEARCH( obj, num, pidx, hmsg ) ((obj)->Search( (COBJECT *)(obj), (num), 0, pidx, MCHK_REG_READ, hmsg ))

/****************************************************************************************************/
/*																									*/
/*		Sqi Function Prototypes																		*/
/*																									*/
/****************************************************************************************************/
void	IxxAnlgMain( 									/* Anlg Main Function						*/
				ANLGV* pAnlg,							/* Analog variables							*/
				struct _token *nmsg );					/* Message token							*/
void	IxxInitAnlg( ANLGV *pAnlg );					/* Initialize Analog 						*/

/* <S03D> Start */
LONG	GetSpeedReference( 								/* Get Speed Reference Anlg					*/
				ANLGV* pAnlg,							/* Analog variables							*/
				int ContrlMode );						/* Control Mode								*/
/* <S03D> End */
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Local Functions 																			*/
/*																									*/
/****************************************************************************************************/
extern void IxxAxctrlReqTrqctrl( MT_CMD *cmdData, long ttrq, long vref );

/****************************************************************************************************/
/*																									*/
/*		Anlg Variables Definition																	*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*  																								*/
/*  Anlg Main Function 																				*/
/*  																								*/
/****************************************************************************************************/
void	IxxAnlgMain( 									/* Anlg Main Function						*/
				ANLGV* pAnlg,							/* Analog variables							*/
				struct _token *nmsg )					/* Message token							*/
{
#define CHK_POT( dpcmd ) ((SqiV.Sigs & SQI_SIGS_POT ) && ((dpcmd) > 0))	/* : /P-OT */
#define CHK_NOT( dpcmd ) ((SqiV.Sigs & SQI_SIGS_NOT ) && ((dpcmd) < 0))	/* : /N-OT */
#define CHK_INHIBIT() (SqiV.Sigs & SQI_SIGS_INHIBIT )					/* : /INHIBIT */
#define CHK_PSEL() (SqiV.Sigs & SQI_SIGS_PSEL )							/* : /PSEL */
LONG dpcmd;
LONG kx,tref,vref;
int req;
/*--------------------------------------------------------------------------------------------------*/
	if ( nmsg->Channel != CHANNEL_ANLG ) 
	{
		pAnlg->LastPcmdCntr = NinV.PcmdRingCntr;					/* ignore input pulse */
		pAnlg->PcmdRem = 0;
		return; 
	}
	else
	{
		req = nmsg->fmt.mes1.arg1;									/* get request */
		nmsg->Channel = CHANNEL_AXCTRL;								/* update motion */
	}
/*--------------------------------------------------------------------------------------------------*/
	/*
	 * get torque reference
	 * filters are as follows:
	 * Pn400: tref gain
	 * Pn415: tref lopass filter
	 */
	kx = (( nmsg->fmt.mes1.arg3 > 0 ) ? pAnlg->TrefKxP : pAnlg->TrefKxN );		   /* get TREF gain */
	tref = MlibMulgain( nmsg->fmt.mes1.arg3, kx );                                 /* set tref */
	tref = MlibLpfilter1( tref, pAnlg->Ktreflpf, pAnlg->TrqRefAd );                /* filter tref */ 
	pAnlg->TrqRefAd = tref;                                                        /* store tref */
/*--------------------------------------------------------------------------------------------------*/
	/*
	 * get velocity reference
	 * filters are as follows:
	 * Pn300: vref gain
	 * Pn307: vref lopass filter
	 */
	vref = MlibMulgain( nmsg->fmt.mes1.arg2, pAnlg->VrefKx );                      /* set vref */
	vref = MlibLpfilter1( vref, pAnlg->Kvreflpf, pAnlg->SpdRefAd );                /* filter vref */ 

/* <S1DD> Delete Start : ���x����݂̂Ń��[�^��]�������]��������������ׁA�R�����g�A�E�g */
#if 0
	/*
		P-CON(Pn50A.0=0�̏ꍇ)�ASPD-D(Pn50A.0=1�̏ꍇ)��ON/OFF��
		���[�^�̉�]�񍐂�I�����鏈���B
		P-CON or SPD-D = OFF:��������]�AP-CON or SPD-D = ON:�t������]�A
		���K�v�ƂȂ����ꍇ�u#if 0�v�A�u#endif�v���R�����g�A�E�g�A�܂��͍폜���ď�����L���ɂ��Ă��������B
	*/
	vref = GetSpeedReference( pAnlg, ANLG_REQ_INTEVELCTRL );
#endif
/* <S1DD> Delete End : ���x����݂̂Ń��[�^��]�������]��������������ׁA�R�����g�A�E�g */

	pAnlg->SpdRefAd = vref;                                                        /* store vref */
/*--------------------------------------------------------------------------------------------------*/
	pAnlg->Tlim = MlibABS( tref );                                                 /* set tlim */
/*--------------------------------------------------------------------------------------------------*/
	dpcmd = nmsg->fmt.mes1.arg4 - pAnlg->LastPcmdCntr;		   /* get distance <S073>			*/
	pAnlg->LastPcmdCntr += dpcmd;							   /* store last pulse counter <S073> */
/*--------------------------------------------------------------------------------------------------*/
	switch ( req )
	{
/*--------------------------------------------------------------------------------------------------*/
	case ANLG_REQ_NOP:
/*--------------------------------------------------------------------------------------------------*/
		IxxAxctrlReqSkip( &(nmsg->fmt.CmdData), MT_HOLD_FASTSTOP );		/* Skip Motion 				*/
		break;
/*--------------------------------------------------------------------------------------------------*/
	case ANLG_REQ_VELCTRL:
/*--------------------------------------------------------------------------------------------------*/
	/* <S1DD> Start */
		/*	Pn50A.0 = 1�̏ꍇ�ASPD-D�M����ON/OFF��					*/
		/*	���[�^�̉�]�����ؑւ����s���鏈���B					*/
		/*	SPD-D = OFF:��������]�A SPD-D = ON:�t������]�A		*/
		vref = GetSpeedReference( pAnlg, ANLG_REQ_VELCTRL );
	/* <S1DD> End */

		tref = (( pAnlg->f.TrefTrqFF ) ? tref : 0 );					/* set tff	 				*/
		IxxAxctrlReqVelctrl( &(nmsg->fmt.CmdData), vref, tref ); 		/* Velctrl Motion 			*/
		break;
/*--------------------------------------------------------------------------------------------------*/
	case ANLG_REQ_TRQCTRL:
/*--------------------------------------------------------------------------------------------------*/
		IxxAxctrlReqTrqctrl( &(nmsg->fmt.CmdData), tref, vref );	    /* Trqctrl Motion 			*/
		break;
/*--------------------------------------------------------------------------------------------------*/
	case ANLG_REQ_INTERPOL:
/*--------------------------------------------------------------------------------------------------*/
//<S073>		dpcmd = nmsg->fmt.mes1.arg4 - pAnlg->LastPcmdCntr;		        /* get distance 			*/
//<S073>		pAnlg->LastPcmdCntr += dpcmd;								    /* store last pulse counter */
/*--------------------------------------------------------------------------------------------------*/
//Mercury		dpcmd = MlibPcmdEgear( dpcmd, &Iprm.EgearX, &pAnlg->PcmdRem );  /* 10 -> 13 */
/*--------------------------------------------------------------------------------------------------*/
		dpcmd = (( CHK_PSEL() ) ? (dpcmd * pAnlg->MulFrq) : dpcmd);		/* @@ /PSEL 				*/
		dpcmd = (( CHK_INHIBIT() ) ? 0 : dpcmd );					 	/* @@ /INHIBIT signal 		*/
//		dpcmd = (( NoutV.Ctrl.CmdCtrlBit & (1<<CLRPOSERR_BITNO)) ? 0 : dpcmd);/* @@  /CLR signal	*/	/* <S1B6> */
/*--------------------------------------------------------------------------------------------------*/
		vref = (( pAnlg->f.VrefSpdFF ) ? vref : 0 );					/* set vff 					*/
		tref = (( pAnlg->f.TrefTrqFF ) ? tref : 0 );					/* set tff	 				*/
/*--------------------------------------------------------------------------------------------------*/
		IxxAxctrlReqInterpolate( &(nmsg->fmt.CmdData), dpcmd, vref, tref );  /* Interpolate Motion 	*/
		break;
/*--------------------------------------------------------------------------------------------------*/

/* <S03D> Start */
	case ANLG_REQ_INTEVELCTRL:
/*--------------------------------------------------------------------------------------------------*/
		tref = (( pAnlg->f.TrefTrqFF ) ? tref : 0 );					/* set tff	 				*/
		/*
		 * get velocity reference
		 * velocity references are as follows:
		 * [Rotary Type]
		 * Pn301: Inter Setting Velocity1
		 * Pn302: Inter Setting Velocity2
		 * Pn303: Inter Setting Velocity3
		 * [Linear Type]
		 * Pn380: Inter Setting Velocity1
		 * Pn381: Inter Setting Velocity2
		 * Pn382: Inter Setting Velocity3
		 */
		vref = GetSpeedReference( pAnlg, ANLG_REQ_INTEVELCTRL );
		IxxAxctrlReqVelctrl( &(nmsg->fmt.CmdData), vref, tref ); 		/* Velctrl Motion 			*/
		break;
/* <S03D> End */

/*--------------------------------------------------------------------------------------------------*/
	default:
/*--------------------------------------------------------------------------------------------------*/
		IxxAxctrlReqSkip( &(nmsg->fmt.CmdData), MT_HOLD_FASTSTOP );		/* Skip Motion */
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*  																								*/
/*  Initialize Anlg 																				*/
/*  																								*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxInitAnlg( ANLGV *pAnlg )
{
#define BRPM0_PNNO (0x0000)	/* <S03D> */
#define BRPM2_PNNO (0x002)
#define VFFOP_PNNO (0x207)
#define MULFRQ_PNNO (0x218)
#define VREFGN_PNNO (0x300)
#define VREFFL_PNNO (0x307)
#define TREFGN_PNNO (0x400)
#define TREFFL_PNNO (0x415)
#define NEARWIDTH_PNNO (0x524)
/* <S03D> Start */
#define INTEVREF1_PNNO (0x301)
#define INTEVREF2_PNNO (0x302)
#define INTEVREF3_PNNO (0x303)
#define INTEVREF1_L_PNNO (0x380)
#define INTEVREF2_L_PNNO (0x381)
#define INTEVREF3_L_PNNO (0x382)
/* <S03D> End */
#define ZADJVL_PNNO (0xE53)	/* <S0F9> */

LONG idx;
//LONG data;
ULONG data;
	/*----------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( pAnlg, sizeof(ANLGV)/4 );		/* reset memory 							*/
	/*----------------------------------------------------------------------------------------------*/
	pAnlg->Tlim = SCANN_DEFTRQ;							/* set initial torque limit					*/
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F �g���N�w�߃��[�h�ǂݏo��(Pn002.0)
	 * ��Pn002.1�͐��䑤
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, BRPM2_PNNO , &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		switch( data & 0x0F )
		{
	/*----------------------------------------------------------------------------------------------*/
		case 0:										/* TFF���蓖�ĂȂ� */
	/*----------------------------------------------------------------------------------------------*/
			pAnlg->f.TrefTrqFF = FALSE;
			break;
	/*----------------------------------------------------------------------------------------------*/
		case 1:										/* TREF���O���g���N�����Ƃ��Ďg�p */
	/*----------------------------------------------------------------------------------------------*/
			pAnlg->f.TrefTrqFF = FALSE;
			break;
	/*----------------------------------------------------------------------------------------------*/
		case 2:										/* TREF���g���N�t�B�[�h�t�H�[���[�h�Ƃ��Ďg�p	*/
	/*----------------------------------------------------------------------------------------------*/
			pAnlg->f.TrefTrqFF = TRUE;
			break;
	/*----------------------------------------------------------------------------------------------*/
		case 3:										/* P-CL, N-CL��ON�̂Ƃ��ATREF���O���ٸ���� 		*/
	/*----------------------------------------------------------------------------------------------*/
			pAnlg->f.TrefTrqFF = FALSE;
			break;
	/*----------------------------------------------------------------------------------------------*/
		default:									/* parameter error 								*/
			pAnlg->f.TrefTrqFF = FALSE;
			break;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F ���xFF�w�߃I�v�V����(Pn207.1)
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, VFFOP_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		switch( (data>>4) & 0x0F )
		{
	/*----------------------------------------------------------------------------------------------*/
		case 0:										/* not use VREF as velocity feed forward		*/
	/*----------------------------------------------------------------------------------------------*/
			pAnlg->f.VrefSpdFF = FALSE;
			break;
	/*----------------------------------------------------------------------------------------------*/
		case 1:										/* use VREF as velocity feed forward			*/
	/*----------------------------------------------------------------------------------------------*/
			pAnlg->f.VrefSpdFF = TRUE;
			break;
	/*----------------------------------------------------------------------------------------------*/
		default: 									/* parameter error 								*/
			break;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn218(�w�߃p���X���͔{��)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, MULFRQ_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn300(���x�w�߃Q�C��)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, VREFGN_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

/* <S03D> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn301(�������x�ݒ�1 ��]�^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, INTEVREF1_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn302(�������x�ݒ�2 ��]�^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, INTEVREF2_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn303(�������x�ݒ�3 ��]�^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, INTEVREF3_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}
/* <S03D> End */

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn307(���x�w�߃��[�p�X�t�B���^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, VREFFL_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

/* <S03D> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn380(�������x�ݒ�1 ���j�A�^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, INTEVREF1_L_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn381(�������x�ݒ�2 ���j�A�^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, INTEVREF2_L_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn382(�������x�ݒ�3 ���j�A�^)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, INTEVREF3_L_PNNO, &(idx), &NetMsg ) )
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}
/* <S03D> End */

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn400(�g���N�w�߃Q�C��)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, TREFGN_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn415(�g���N�w�߃t�B���^���萔)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, TREFFL_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn524(/NEAR��)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, NEARWIDTH_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}

/* <S03D> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F Pn000.1(���䃂�[�h�I��)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, BRPM0_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}
	NoutV.CtrlModeSel = (data>>4) & 0x0F;
/* <S03D> End */

/* <S0F9> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * �b��F PnE53(���x�w�߃[����)���Čv�Z����
	 */
	/*----------------------------------------------------------------------------------------------*/
	if( OK == OBJ_SEARCH( CPARAMX, ZADJVL_PNNO, &(idx), &NetMsg ) ) 
	{
		OBJ_READ( CPARAMX, idx, &data, &NetMsg );
		OBJ_WRITE( CPARAMX, idx, data, &NetMsg );
	}
	NoutV.CtrlModeSel = (data>>4) & 0x0F;
/* <S0F9> End */

	return;
}
#pragma arm section code

/* <S03D> Start */
/****************************************************************************************************/
/*  																								*/
/*  Get Speed Reference Anlg 																		*/
/*				[parameters]																		*/
/*					ANLGV* pAnlg : Analog variables													*/
/*					int ContrlMode : Control Mode													*/
/*				[return value]																		*/
/*					vref : Speed Reference															*/
/*  																								*/
/****************************************************************************************************/
LONG	GetSpeedReference( ANLGV* pAnlg, int ContrlMode )
{

LONG vref;
//NINV *pninv = &NinV;	/* set signal assign mode(=Pn50A.0�̒l) */

UINT spd_d;		/* SPD-D, P-CON */
UINT spd_a;		/* SPD-A, P-CL */
UINT spd_b;		/* SPD-B, N-CL */


	switch ( ContrlMode )
	{

	/* Velocity Control */
	case ANLG_REQ_VELCTRL:
/* <S162> */
//		if(pninv->P.AssignMode == SQI_ASSIGN_DEFAULT)
//		{	/* Pn50A.0 = 0 */
//			/* Get Rotation Direction Signal(=P-CON) */
//			spd_d = ((SqiV.Sigs & SQI_SIGS_PCON ) >> SQI_BNO_PCON);	/* P-CON */
//		}
//		else
		{	/* Pn50A.0 = 1 */
			/* Get Rotation Direction Signal(=SPD-D) */
			spd_d = ((SqiV.Sigs & SQI_SIGS_SPDD ) >> SQI_BNO_SPDD);	/* SPD-D */
		}

		/* Set Speed */
//		vref = (( spd_d ) ? (-vref) : vref);								/* <S1DD> */
		vref = (( spd_d ) ? ((-1) * (pAnlg->SpdRefAd)) : pAnlg->SpdRefAd);	/* <S1DD> */
		break;

	/* Inter Velocity Control */
	case ANLG_REQ_INTEVELCTRL:
/* <S162> */
//		if(pninv->P.AssignMode == SQI_ASSIGN_DEFAULT)
//		{	/* Pn50A.0 = 0 */
//			/* Select Inter SPeed Level */
//			spd_d = ((SqiV.Sigs & SQI_SIGS_PCON ) >> SQI_BNO_PCON);	/* P-CON */
//			spd_a = ((SqiV.Sigs & SQI_SIGS_PCL ) >> SQI_BNO_PCL);	/* P-CL */
//			spd_b = ((SqiV.Sigs & SQI_SIGS_NCL ) >> SQI_BNO_NCL);	/* N-CL */
//		}
//		else
		{	/* Pn50A.0 = 1 */
			/* Select Inter SPeed Level */
			spd_d = ((SqiV.Sigs & SQI_SIGS_SPDD ) >> SQI_BNO_SPDD);	/* SPD-D */
			spd_a = ((SqiV.Sigs & SQI_SIGS_SPDA ) >> SQI_BNO_SPDA);	/* SPD-A */
			spd_b = ((SqiV.Sigs & SQI_SIGS_SPDB ) >> SQI_BNO_SPDB);	/* SPD-B */
		}

		/* Set Speed */
//		vref = pAnlg->InterSpdLevel[ (spd_a<<1) | spd_b ];	/* <S0CE> */
		vref = pAnlg->InterSpdLevel[ (spd_b<<1) | spd_a ];	/* <S0CE> */
		vref = (( spd_d ) ? (-vref) : vref);
		break;

	default:
		vref = 0;			/* ���������ϐ��C��<S191> */
		break;
	}

	return(vref);

}
/* <S03D> End */


/****************************************************************************************************/
/*  																								*/
/*  Input Torque Reference from AD value <S051>	 													*/
/*	return: Torque Reference [2^24/MaxTrq]															*/
/*  																								*/
/****************************************************************************************************/
LONG	NpiInputTrqRefAd( LONG Adx )
{
LONG tref;
KSGAIN kx;
ANLGV	*pAnlg = &AnlgV;
	/*
	 * get torque reference
	 * filters are as follows:
	 * Pn400: tref gain
	 * Pn415: tref lopass filter
	 */
	kx = (( Adx > 0 ) ? pAnlg->TrefKxP : pAnlg->TrefKxN );		                   /* get TREF gain */
	tref = MlibMulgain( Adx, kx );                                                 /* set tref */
	return ( tref );                                                               /* return tref */
}

