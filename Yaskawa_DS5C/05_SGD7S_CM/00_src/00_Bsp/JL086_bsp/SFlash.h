/************************************************************************************************************/
/*																											*/
/*																											*/
/*		SFlash.h : SoC(JL-086)�V���A��Flash�������R���g���[���֘A����										*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		�V���A��Flash�������R���g���[���֘A�����\�[�X�t�@�C��												*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.29  Y.Tanaka		����															*/
/*																											*/
/************************************************************************************************************/
#if	!defined( SRROM_H__ )
#define	SRROM_H__

#define SFMEMC_Init()	ARMPF_REG_WRITE( ARMPF_SFMEMC_SFMSMD,													/* �ʏ탂�[�h	*/\
					SFMSMD_SLOWREAD|SFMSMD_SPIEXT0|SFMSMD_NO_PREFETCH|SFMSMD_ABORT_DISABLE\
					|SFMSMD_SPIMODE0|SFMSMD_OEEXT0|SFMSMD_MOSUPWAIT0|SFMSMD_MOHLDWAIT0 );\
	ARMPF_REG_WRITE( ARMPF_SFMEMC_SFMSSC,													/* SSB�ŏ��n�C���F8SCK	*/\
					SFMSSC_SSB_H_DELAY1 | SFMSSC_SSB_L_DELAY1 | 0x07 );\
	ARMPF_REG_WRITE( ARMPF_SFMEMC_SFMSKC, SFMSKC_DUTY50 | 0x06);							/* SCK���x�F8����	*/\

#define	SFM_Transaction_open()	ARMPF_REG_WRITE( ARMPF_SFMEMC_SFMCMD, SFMCMD_DCOM_ENABLE );				/* DOPE����	*/

#define	SFM_Transaction_close()	ARMPF_REG_WRITE( ARMPF_SFMEMC_SFMCMD, SFMCMD_DCOM_DISABLE );				/* DOPE�֎~	*/

#define	SFM_GetByte()				((UCHAR)ARMPF_REG_READ( ARMPF_SFMEMC_SFMCOM ))

#define	SFM_PutByte( cVal )		ARMPF_REG_WRITE( ARMPF_SFMEMC_SFMCOM, cVal )


////////////////////////////////////////////////////////////////
// �v���g�^�C�v
void	SerialRomInit( void );
void	SerialRomWriteEnable( void );
void	SerialRomWriteDisable( void );
LONG	SerialRomReadID( void );
LONG	SerialRomReadStatusRegister( void );
void	SerialRomWaitReady( void );
void	SerialRomChipErase( void );
void	SerialRomSectorErase( ULONG lAddr );
void	SerialRomPageProgram( ULONG lAddr, ULONG *pBuff, LONG nSize );
void	SerialRomReadDataBytes( ULONG lAddr, UCHAR *pBuff, LONG nSize );
void	SerialRomReadDataBytes4( ULONG lAddr, ULONG *pBuff, LONG nSize );
LONG	SerialRomCheckDataBytes( ULONG lAddr, UCHAR *pBuff, LONG nSize );
void	SerialRomDeepPowerDown( void );
void	SerialRomReleaseDeepPowerDown( void );
UCHAR	SFlashChkStatus( void );	/* tanaka21 add 2013.07.09 */
#endif
/*-------end of file----------------------------------------------------------------------------------------*/
