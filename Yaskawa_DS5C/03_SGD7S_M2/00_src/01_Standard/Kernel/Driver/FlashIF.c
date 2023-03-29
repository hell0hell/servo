/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FlashIF.c : Flash�����C���^�t�F�[�X															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : SoC������уI�v�V�������W���[����Flash�̂h�e�������s���B								*/
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
/*	Note	:	����	2013.07.09	Y.Tanaka														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "FlashIF.h"
#include "KLib.h"


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
void	InitSFlashSoC( void );				/* �{��SerialFlash������		*/
UCHAR	SFlashModeChgSoC( UCHAR SPI, UCHAR Read, UCHAR PreFetch, UCHAR Com );		/* �{��SerialFlash���[�h�ύX		*/
LONG	SFlashEraseSectorSoC( ULONG EraseAddr );	/* �{��SerialFlash�Z�N�^�[����	*/
LONG	SFlashWritePageSoC( ULONG WriteAddr, ULONG *WriteBuf );		/* �{��SerialFlash�y�[�W���C�g	*/
LONG	SFlashReadByteSoC( ULONG ReadAddr, UCHAR *ReadBuf );		/* �{��SerialFlash�o�C�g���[�h	*/
//LONG	SFlashEraseSectorOption( ULONG EraseAddr );		/* �I�v�V�������SerialFlash�u���b�N����(DPM)	*/
//LONG	SFlashWritePageOption( ULONG WriteAddr, ULONG *WriteBuf );	/* �I�v�V�������SerialFlash�y�[�W���C�g(DPM)	*/

/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/
typedef struct{
	struct{
		UCHAR	SPI;	/* SPImode 0/1 - SPI0/SPI3								*/
		UCHAR	Read;	/* read mode 0/1 - normal/fast							*/
		UCHAR	PreFetch;	/* pre fetch mode 0/1 - off/on						*/
		UCHAR	Com;	/* Communication mode 0/1 - rom/direct					*/
	}Mode;
	struct{
		UCHAR	erase;	/* erase status 0/1 - none/busy							*/
		UCHAR	write;	/* write status 0/1 - none/busy							*/
		UCHAR	read;	/* read status 0/1 - none/busy							*/
		UCHAR	Comm;	/* any accese status 0/1 - none/busy					*/
	}Sts;
}SFLASHVAL;
static	SFLASHVAL	SFlashV;


/****************************************************************************************************/
/*																									*/
/*		�{��SerialFlash������																		*/
/*																									*/
/****************************************************************************************************/
void InitSFlashSoC( void )
{
	SerialRomInit( );
#ifdef	_MERCURY_PLATFORM_			/*<S190>*/
    MlibResetByteMemory( &SFlashV, sizeof(SFlashV) );
#endif	/*_MERCURY_PLATFORM_*/
}


/****************************************************************************************************/
/*																									*/
/*		�{��SerialFlash���[�h�ύX																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/****************************************************************************************************/
UCHAR	SFlashModeChgSoC( UCHAR SPI, UCHAR Read, UCHAR PreFetch, UCHAR Com )
{
	UCHAR	rc;
	
	SFlashV.Mode.SPI = SPI;
	SFlashV.Mode.Read = Read;
	SFlashV.Mode.PreFetch = PreFetch;
	SFlashV.Mode.Com = Com;
//	rc = SerialRomModeChange( SFlashV.Mode );
	rc = 0;
	
	return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		�{��SerialFlash�Z�N�^�[����																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSerialFlash�̃Z�N�^�[�����R�}���h���s/�I���Ď�										*/
/*		�����F																						*/
/*			ULONG EraseAddr	:��������Z�N�^�[�̐擪�A�h���X(0xFFE00000����64KB(0x10000)����)		*/
/*		�߂�l�FOK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:�Z�N�^�[��������I��																	*/
/*			BUSY:Erase���s��(�p��)																	*/
/*			NG_ADDR:�ݒ�A�h���X�ُ�																*/
/*			NG_CMD:���R�}���h(write/read)���s��														*/
/*																									*/
/****************************************************************************************************/
LONG	SFlashEraseSectorSoC( ULONG EraseAddr )
{
	LONG rc;		/* -1/0/1 - NG/OK/BUSY	*/

	if( EraseAddr < 0xFFE00000 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else if( (EraseAddr & 0x0000FFFF) != 0x0 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else
	{
		if( SFlashV.Sts.Comm == 0/*NONE*/ )	/* ���s���R�}���h�Ȃ� */
		{
			/* Sector Erase */
			SFlashV.Sts.Comm = 1/*BUSY*/;
			SFlashV.Sts.erase = 1/*BUSY*/;
			SerialRomSectorErase( EraseAddr );
			rc = 1/*BUSY*/;
		}
		else	/* ���s���R�}���h���� */
		{
			if( SFlashV.Sts.erase == 1/*BUSY*/ )	/* Erase���s�� */
			{
				SFlashV.Sts.erase = (SFlashChkStatus()&0x01);		/* Erase���s��Ԋm�F */
				if( SFlashV.Sts.erase == 0 )		/* Erase�I�� */
				{
					SFlashV.Sts.Comm = 0/*OK*/;
					rc = 0/*OK*/;
				}
				else
				{
					rc = 1/*BUSY*/;
				}
			}
			else	/* ���̏������s�� */
			{
				rc = -2/*NG_CMD*/;
			}
		}
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		�{��SerialFlash�y�[�W���C�g																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�w�肳�ꂽMPU��SerialFlash�y�[�W���C�g�R�}���h���s									*/
/*		�����F																						*/
/*			ULONG WriteAddr	:�������ރy�[�W�̐擪�A�h���X											*/
/*			ULONG* WriteBuf	:�������݃f�[�^�o�b�t�@													*/
/*		�߂�l�FOK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK		:����I��																		*/
/*			BUSY	:���s��(�p��)																	*/
/*			NG_ADDR	:�ݒ�A�h���X�ُ�																*/
/*			NG_CMD	:���R�}���h(write/read)���s��													*/
/*																									*/
/****************************************************************************************************/
LONG	SFlashWritePageSoC( ULONG WriteAddr, ULONG *WriteBuf )
{
	LONG rc;		/* -1/0/1 - NG/OK/BUSY	*/

	if( WriteAddr < 0xFFE00000 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else if( (WriteAddr & 0x000000FF) != 0x0 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else
	{
		if( SFlashV.Sts.Comm == 0/*NONE*/ )	/* ���s���R�}���h�Ȃ� */
		{
			/* Page Program */
			SFlashV.Sts.Comm = 1/*BUSY*/;
			SFlashV.Sts.write = 1/*BUSY*/;
			SerialRomPageProgram( WriteAddr, WriteBuf, 0x40 );
			rc = 1/*BUSY*/;
		}
		else	/* ���s���R�}���h���� */
		{
			if( SFlashV.Sts.write == 1/*BUSY*/ )	/* Write���s�� */
			{
				SFlashV.Sts.write = (SFlashChkStatus()&0x01);		/* Write���s��Ԋm�F */
				if( SFlashV.Sts.write == 0 )		/* Write�I�� */
				{
					SFlashV.Sts.Comm = 0/*OK*/;
					rc = 0/*OK*/;
				}
				else
				{
					rc = 1/*BUSY*/;
				}
			}
			else	/* ���̏������s�� */
			{
				rc = -2/*NG_CMD*/;
			}
		}
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		�{��SerialFlash�o�C�g���[�h																	*/
/*																									*/
/****************************************************************************************************/
LONG	SFlashReadByteSoC( ULONG ReadAddr, UCHAR *ReadBuf )
{
	INT nSize;
	UCHAR pBuff[10] = {0};

	nSize = sizeof(pBuff);
	SerialRomReadDataBytes( ReadAddr, pBuff, nSize );
}



