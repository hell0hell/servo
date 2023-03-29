/************************************************************************************************************/
/*																											*/
/*																											*/
/*		Interrupt.c : Mercury ���荞�݊֘A����																*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		���荞�ݐݒ�\�[�X�t�@�C��																			*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.14  Y.Tanaka		����															*/
/*																											*/
/************************************************************************************************************/
#include	"BaseDef.h"
#include	"ARMPF_VIC.h"
#include	"Interrupt.h"
#include	"Klib.h"
#include	"HwDeviceIf.h"

/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
void 	InitInterrupt( void );							/* ���荞�݊֘A����������				*/
void 	InitInterruptStart( void );						/* ���荞�݃X�^�[�g						*/
void 	IntIgnore( void );									/* �������[�v���荞��					*/
void 	Common_IRQHandler0( void );						/* ARMPF_VIC_ISC0�p���ʊ��荞�݃n���h��	*/
void 	Common_IRQHandler1( void );						/* ARMPF_VIC_ISC1�p���ʊ��荞�݃n���h��	*/
void 	ScanAIntHandler( void );							/* ScanA���荞�ݑO�㏈��				*/
void 	ScanBIntHandler( void );							/* ScanB���荞�ݑO�㏈��				*/
void 	ScanCIntHandler( void );							/* ScanC���荞�ݑO�㏈��				*/
static	ULONG SearchIRQReqBit( VULONG value );			/* ���荞�ݗv�����o����					*/
BOOL	GetIntUDLStatus( UCHAR num );						/* INTUDL*�����ݗv���擾����			*///<S0C7>
void 	ReqIntUDLStatusClear( UCHAR num );				/* INTUDL*�����ݗv���N���A����			*///<S0C7>

/****************************************************************************************************/
/*																									*/
/*		�O���֐�extern�錾																			*/
/*																									*/
/****************************************************************************************************/
extern void SysScanAInt( void );
extern void SysScanBInt( void );
extern void SysScanCInt( void );
extern void __SET_VE( void );


/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/

#ifdef _MERCURY_PLATFORM_	/* �Í����t�H�[�}�b�g�Ή� <S146> */
/****************************************************************************************************/
/*																									*/
/*		���荞�݊֘A����������																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FVIC�̃��W�X�^�ݒ�A���荞�݊֐��Z�b�g													*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void InitInterrupt(void)
{
	LONG i,j;
	ULONG PlsRegValue[2] = {0};
	ULONG EdgcRegValue[4] = {0};
	ULONG LvlRegValue[4] = {0};

//	__DI();

//	InitializeVIC();
	/* ���荞�݃C�l�[�u���N���A/�G�b�W���o�r�b�g�N���A�ݒ�	*/
	for(i=0;i<2;i++)
	{
		ARMPF_REG_WRITE( (ARMPF_VIC_IEC0 + (i*4)), ARMPF_LONGALL1 );
		ARMPF_REG_WRITE( (ARMPF_VIC_PIC0 + (i*4)), ARMPF_LONGALL1 );
	}
	/* ���荞�݂����ׂ�IRQ�ɐݒ�	*/
	ARMPF_REG_WRITE( ARMPF_VIC_ISL0, ARMPF_LONGALL0 );
	ARMPF_REG_WRITE( ARMPF_VIC_ISL1, ARMPF_LONGALL0 );

	/* �e�[�u���f�[�^��ǂݍ����VIC�ɐݒ�	*/
	/* ���荞�݌��o���@�ݒ�	*/
	for(i=0;i<2;i++)
	{
		for(j=0;j<32;j++)
		{
			PlsRegValue[i] |= (InitIntVecTbl[ j+i*32 ].PLS << j);
		}
		ARMPF_REG_WRITE( (ARMPF_VIC_PLS0 + (i*4)) , PlsRegValue[i] );		/* ���x��/�G�b�W�ݒ�	*/
	}
}
#else	/* _MERCURY_PLATFORM_ */	/* �Í����t�H�[�}�b�g�Ή� <S146> */

#pragma arm section code = "INIT_VECT_CODE"
/****************************************************************************************************/
/*																									*/
/*		���荞�݊֘A����������																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FVIC�̃��W�X�^�ݒ�A���荞�݊֐��Z�b�g													*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void InitInterrupt(void)
{
	LONG i,j;
	ULONG PlsRegValue[2] = {0};
	ULONG EdgcRegValue[4] = {0};
	ULONG LvlRegValue[4] = {0};

//	__DI();

//	InitializeVIC();
	/* ���荞�݃C�l�[�u���N���A/�G�b�W���o�r�b�g�N���A�ݒ�	*/
	for(i=0;i<2;i++)
	{
		ARMPF_REG_WRITE( (ARMPF_VIC_IEC0 + (i*4)), ARMPF_LONGALL1 );
		ARMPF_REG_WRITE( (ARMPF_VIC_PIC0 + (i*4)), ARMPF_LONGALL1 );
	}
	/* ���荞�݂����ׂ�IRQ�ɐݒ�	*/
	ARMPF_REG_WRITE( ARMPF_VIC_ISL0, ARMPF_LONGALL0 );
	ARMPF_REG_WRITE( ARMPF_VIC_ISL1, ARMPF_LONGALL0 );

	/* �e�[�u���f�[�^��ǂݍ����VIC�ɐݒ�	*/
	/* ���荞�݌��o���@�ݒ�	*/
	for(i=0;i<2;i++)
	{
		for(j=0;j<32;j++)
		{
			PlsRegValue[i] |= (InitIntVecTbl[ j+i*32 ].PLS << j);
		}
		ARMPF_REG_WRITE( (ARMPF_VIC_PLS0 + (i*4)) , PlsRegValue[i] );		/* ���x��/�G�b�W�ݒ�	*/
	}

	for(i=0;i<4;i++)
	{
		for(j=0;j<16;j++)
		{
			if(InitIntVecTbl[j+i*16].PLS == ARMPF_VIC_EDGE)
			{/*�G�b�W���o�̏ꍇ*/
				EdgcRegValue[i] |= (InitIntVecTbl[ j+i*16 ].EDGC << (j*2));
			}
			else if(InitIntVecTbl[j+i*16].PLS == ARMPF_VIC_LVL)
			{/*���x�����o�̏ꍇ*/
				LvlRegValue[i] |= (InitIntVecTbl[ j+i*16 ].EDGC << (j*2));
			}
		}
		ARMPF_REG_WRITE( (ARMPF_VIC_EDGC0 + (i*4)) , EdgcRegValue[i] );		/* �G�b�W���o���@�ݒ�	*/
		ARMPF_REG_WRITE( (ARMPF_VIC_LVLC0 + (i*4)) , LvlRegValue[i] );		/* ���x�����o���@�ݒ�	*/
	}

	/* ���荞�ݗD�惌�x���}�X�N���ׂĉ���	*/
	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, ARMPF_WORDALL1 );

	/* ���荞�ݗD��x/���荞�݃n���h���ݒ�	*/
	for(i=0;i<64;i++)
	{
		ARMPF_REG_WRITE( (ARMPF_VIC_PRL0 + (i*4)), IntVecTbl[i].Priority );	/* ���荞�ݗD��x�ݒ�	*/
		ARMPF_REG_WRITE( (ARMPF_VIC_VAD0 + (i*4)), (ULONG)(InitIntVecTbl[i].RegHandler) );	/* ���荞�݃n���h���ݒ�	*/
	}

	/* ���荞�݂̋���/�}�X�N��ݒ�	*/
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x00000007 );	/* UDL���荞�݂̂ݗL�� for debug */
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x003C0007 );	/* UDL+IntervalTimer���荞�݂̂ݗL�� for debug */
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x003C0000 );	/* IntervalTimer���荞�݂̂ݗL�� for debug */
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN1, 0x0FDE00C0 );
	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, ARMPF_LONGALL0 );
	ARMPF_REG_WRITE( ARMPF_VIC_IEN1, ARMPF_LONGALL0 );

	__SET_VE();		/* VIC�L��			*/
}


/****************************************************************************************************/
/*																									*/
/*		���荞�݊J�n����																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F���荞�ݗv���N���A��A���荞�݋���													*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void InitInterruptStart(void)
{


	/* ���荞�݃G�b�W���o�r�b�g�N���A�ݒ�	*/
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, ARMPF_LONGALL1 );
	ARMPF_REG_WRITE( ARMPF_VIC_PIC1, ARMPF_LONGALL1 );		/*<S008>*/

	/* CP15 �����ݗL��	*/
	__EI();



	/* ���荞�݃G�b�W���o�r�b�g�N���A�ݒ�	*/
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, ARMPF_LONGALL1 );
	ARMPF_REG_WRITE( ARMPF_VIC_PIC1, ARMPF_LONGALL1 );		/*<S008>*/

	/* ���荞�݂̋���/�}�X�N��ݒ�	*//* VIC_IEN0��L���ɂ���Ƃ�����Scan�������o���̂�VIC_IEN1���Ƀ}�X�N�������� <S008>*/
#if 0	/* <S150> USB���荞�݂��g�p���Ȃ�(�|�[�����O��) */
	ARMPF_REG_WRITE( ARMPF_VIC_IEN1, 0x000000C0 );				/* USB�����݂̂ݗL��*/
#endif
	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x00000007 | (1<<29) );	/* UDL���荞�݂�I2C���荞�݂̂ݗL�� for debug */

}

/****************************************************************************************************/
/*																									*/
/*		�������[�v																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�K�肵�Ă��Ȃ����荞�݂Ŏ��s															*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void IntIgnore(void)
{
	while(1);
}


#pragma arm section code = "VECT_CODE"
/****************************************************************************************************/
/*																									*/
/*		ARMPF_VIC_ISC0�p���ʊ��荞�݃n���h��														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FARMPF_VIC_ISC0�ɓo�^����Ă��銄�荞�݂̗v�������ƃn���h���Ăяo��					*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void Common_IRQHandler0( void )
{
	ULONG	IRQNumber;
	ULONG	priority;
	ULONG	mask;

//IRQHandler�Ŋ��荞�݋֎~�ɂ��Ă���̂ŕs�v
//	/* __DI */

	/* ���荞�ݗv���r�b�g�������Ă���Ƃ����T����IRQNumber�ɃZ�b�g���� */
	IRQNumber = SearchIRQReqBit(ARMPF_REG_READ(ARMPF_VIC_ISC0));	/* ARMPF_VIC_ISC0�̃r�b�g�������Ă���Ƃ�������� */
	if( IRQNumber < 32 )		/* ARMPF_VIC_ISC0�Ɋ��荞�ݗv�������������ꍇ */
	{
		/* ���荞�ݗv�����N���A */
		ARMPF_REG_WRITE( ARMPF_VIC_PIC0, (0x1 << IRQNumber) );

		/* �����D��x�ȉ��̊��荞�݂��}�X�N */
		priority = (0x1 << IntVecTbl[IRQNumber].Priority);
		mask = ( priority ^ (-priority) );	/* ���D��x�ȉ��̃r�b�g�𗧂Ă�		*/
		ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

		__EI();
		IntVecTbl[IRQNumber].IntHandler();		/* ���荞�݃n���h���R�[��		*/
		__DI();

		/* �����D��x�ȉ��̊��荞�݂��}�X�N�N���A */
		ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );
	}

	/* ���荞�ݏ����I����ʒm(HVA�Ƀ_�~�[���C�g) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, IRQNumber );
	__EI();

}


/****************************************************************************************************/
/*																									*/
/*		ARMPF_VIC_ISC1�p���ʊ��荞�݃n���h��														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FARMPF_VIC_ISC1�ɓo�^����Ă��銄�荞�݂̗v�������ƃn���h���Ăяo��					*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void Common_IRQHandler1( void )
{
	ULONG	IRQNumber;
	ULONG	priority;
	ULONG	mask;

//IRQHandler�Ŋ��荞�݋֎~�ɂ��Ă���̂ŕs�v
//	/* __DI */

	/* ���荞�ݗv���r�b�g�������Ă���Ƃ����T����IRQNumber�ɃZ�b�g���� */
	IRQNumber = SearchIRQReqBit(ARMPF_REG_READ(ARMPF_VIC_ISC1));	/* ARMPF_VIC_ISC1�̃r�b�g�������Ă���Ƃ�������� */
	if( IRQNumber < 32 )		/* ARMPF_VIC_ISC1�Ɋ��荞�ݗv�������������ꍇ */
	{
		/* ���荞�ݗv�����N���A */
		ARMPF_REG_WRITE( ARMPF_VIC_PIC1, (0x1 << IRQNumber) );

		/* �����D��x�ȉ��̊��荞�݂��}�X�N */
		priority = (0x1 << IntVecTbl[IRQNumber+32].Priority);		/*<S00B>*/
		mask = ( priority ^ (-priority) );	/* ���D��x�ȉ��̃r�b�g�𗧂Ă�		*/
		ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

		__EI();
		IntVecTbl[IRQNumber+32].IntHandler();		/* ���荞�݃n���h���R�[��		*/		/*<S008>*/
		__DI();

		/* �����D��x�ȉ��̊��荞�݂��}�X�N�N���A */
		ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );
	}

	/* ���荞�ݏ����I����ʒm(HVA�Ƀ_�~�[���C�g) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, IRQNumber );
	__EI();

}


/****************************************************************************************************/
/*																									*/
/*		ScanA���荞�ݑO�㏈��																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FScanA���荞�ݗv���̃N���A/�}�X�N�Ȃǂ̑O�㏈��										*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void ScanAIntHandler( void )
{
//	ULONG	priority;
//	ULONG	mask;

	HALdrv_OutputDebugSP( INTERRUPT_ScanA );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */

//IRQHandler�Ŋ��荞�݋֎~�ɂ��Ă���̂ŕs�v
//	/* __DI */

	/* ���荞�ݗv�����N���A */
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, 0x01 );

	/* �����D��x�ȉ��̊��荞�݂��}�X�N *//* �폜,  <S0E2> */
//	priority = (0x1 << IntVecTbl[0].Priority);
//	mask = ( priority ^ (-priority) );	/* ���D��x�ȉ��̃r�b�g�𗧂Ă�		*/
//	ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

//	__EI(); /* <S01E> */
	SysScanAInt();		/* ���荞�݃n���h���R�[��		*/
//	__DI(); /* <S01E> */

	/* �����D��x�ȉ��̊��荞�݂��}�X�N�N���A *//* <S0E2> */
//	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );

	/* ���荞�ݏ����I����ʒm(HVA�Ƀ_�~�[���C�g) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, 0 );
//	__EI(); /* <S01E> */

	HALdrv_ClearDebugSP( INTERRUPT_ScanA );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */

}


/****************************************************************************************************/
/*																									*/
/*		ScanB���荞�ݑO�㏈��																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FScanB���荞�ݗv���̃N���A/�}�X�N�Ȃǂ̑O�㏈��										*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void ScanBIntHandler( void )
{
	ULONG	priority;
	ULONG	mask;

	HALdrv_OutputDebugSP( INTERRUPT_ScanB );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */

//IRQHandler�Ŋ��荞�݋֎~�ɂ��Ă���̂ŕs�v
//	/* __DI */

	/* ���荞�ݗv�����N���A */
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, 0x02 );

	/* �����D��x�ȉ��̊��荞�݂��}�X�N */
	priority = (0x1 << IntVecTbl[1].Priority);
	mask = ( priority ^ (-priority) );	/* ���D��x�ȉ��̃r�b�g�𗧂Ă�		*/
	ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

	__EI();
	SysScanBInt();		/* ���荞�݃n���h���R�[��		*/
	__DI();

	/* �����D��x�ȉ��̊��荞�݂��}�X�N�N���A */
	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );

	/* ���荞�ݏ����I����ʒm(HVA�Ƀ_�~�[���C�g) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, 1 );
//	__EI(); /* <S01E> */

	HALdrv_ClearDebugSP( INTERRUPT_ScanB );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */

}


/****************************************************************************************************/
/*																									*/
/*		ScanC���荞�ݑO�㏈��																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FScanC���荞�ݗv���̃N���A/�}�X�N�Ȃǂ̑O�㏈��										*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void ScanCIntHandler( void )
{
	ULONG	priority;
	ULONG	mask;

#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
	HALdrv_OutputDebugSP( INTERRUPT_ScanC );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */
#endif	//#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE

//IRQHandler�Ŋ��荞�݋֎~�ɂ��Ă���̂ŕs�v
//	/* __DI */

	/* ���荞�ݗv�����N���A */
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, 0x04 );

	/* �����D��x�ȉ��̊��荞�݂��}�X�N */
	priority = (0x1 << IntVecTbl[2].Priority);
	mask = ( priority ^ (-priority) );	/* ���D��x�ȉ��̃r�b�g�𗧂Ă�		*/
	ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

	__EI();
	SysScanCInt();		/* ���荞�݃n���h���R�[��		*/
	__DI();

	/* �����D��x�ȉ��̊��荞�݂��}�X�N�N���A */
	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );

	/* ���荞�ݏ����I����ʒm(HVA�Ƀ_�~�[���C�g) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, 2 );
//	__EI(); /* <S01E> */

#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
	HALdrv_ClearDebugSP( INTERRUPT_ScanC );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */
#endif	//#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE

}


/****************************************************************************************************/
/*																									*/
/*		���荞�ݗv�����o����																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FARMPF_VIC_ISCx(x=0,1)���犄�荞�ݗv���������Ă�����̂�T��							*/
/*		�����FARMPF_VIC_ISCx�̒l																	*/
/*		�߂�l�FARMPF_VIC_ISCx�Ńr�b�g�������Ă���Ƃ���(0�`31)										*/
/*				���������Ă��Ȃ��ꍇ��32															*/
/*																									*/
/****************************************************************************************************/
static	ULONG SearchIRQReqBit(VULONG value)
{
	ULONG	rc;
	
	rc = (value & (-value)) - 1;
    rc = (rc & 0x55555555) + (rc >> 1 & 0x55555555);
    rc = (rc & 0x33333333) + (rc >> 2 & 0x33333333);
    rc = (rc & 0x0f0f0f0f) + (rc >> 4 & 0x0f0f0f0f);
    rc = (rc & 0x00ff00ff) + (rc >> 8 & 0x00ff00ff);
    return (rc & 0x0000ffff) + (rc >>16 & 0x0000ffff);
}

/****************************************************************************************************/
/*																									*/
/*	INTUDL*�����ݗv���擾����																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F���荞�ݓ��̓X�e�[�^�X�E���W�X�^����UDL�̊����ݏ�Ԃ��擾����							*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�FTRUE or FALSE																		*/
/*																									*/
/****************************************************************************************************/
BOOL GetIntUDLStatus(UCHAR num)
{
	BOOL	rc;
	VULONG	sts;
	
	sts = ARMPF_REG_READ(ARMPF_VIC_RAIS0);
	if((sts >> num) & 0x00000001)
	{/* SyncStatus	*/
		rc = TRUE;
	}
	else
	{/* UnSyncStatus	*/
		rc = FALSE;
	}
	return rc;
}

/****************************************************************************************************/
/*																									*/
/*	INTUDL*�����ݗv���N���A����																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FUDL�̊����݂��N���A����																*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�FTRUE or FALSE																		*/
/*																									*/
/****************************************************************************************************/
void ReqIntUDLStatusClear(UCHAR num)
{
	VULONG	sts;
	
	sts = ARMPF_REG_READ(ARMPF_VIC_PIC0);
	sts = (sts | (0x01 << num));
	ARMPF_REG_WRITE(ARMPF_VIC_PIC0, sts);
	
	return;
}
#endif /* _MERCURY_PLATFORM_ */	/* �Í����t�H�[�}�b�g�Ή� <S146> */
/*-------end of file----------------------------------------------------------------------------------------*/
