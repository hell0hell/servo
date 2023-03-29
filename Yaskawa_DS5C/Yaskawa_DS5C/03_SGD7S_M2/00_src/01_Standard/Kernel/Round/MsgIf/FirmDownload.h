/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FirmDownload.h : MEMOBUS�t�@�[���_�E�����[�h�Ǘ�											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : MEMOBUS�t�@�[���_�E�����[�h�ʐM�h�e�������s���B											*/
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
/*	Note	:	����	2013.07.08	Y.Tanaka	For Mercury											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/




/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�_�E�����[�h������`																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MPUNO		0x00						/* �h�e����CPU��MPU #0�Ƃ���				*/
#define	FLASH_NUM	0x01						/* �t���b�V����������							*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS1	0x0001						/* bit0=1	:�V�X�e���_�E�����[�h�Ή�			*/
													/* bit1=0	:�@��ID�R�[�h����					*/
													/* bit2-15	:�\��								*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS2	0x0001						/* bit0=1	:#0 Flash �V�X�e���_�E�����[�h�Ή�	*/
													/* bit1-15	:#1-#15 Flash �Ȃ�					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS3	0x0000						/* bit0=1	:#0 Flash ��̧���ID�L��				*/
													/* bit1-15	:#1-#15 Flash �Ȃ�					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS4	0x0000						/* bit0=1	:#0 Flash ���޲�ID�L��				*/
													/* bit1-15	:#1-#15 Flash �Ȃ�					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_WAY	0x0000						/* bit0-3=0	:�t���b�V�������ݕ��@�I��			*/
													/*				=0:�ʐM�ƃt���b�V�������ݕ��s	*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_1DLEN	0x0100						/* �P���b�Z�[�W������̏����o�C�g��(=256byte)	*/


/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�_�E�����[�h�R�}���h�֘A��`																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS �t�@�[���_�E�����[�h	�T�u�t�@���N�V�����R�[�h									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_DL_ID_RD			0x00			/* ID���ǂݏo��									*/
#define	MBUS_DL_SWVER_RD		0x01			/* �o�[�W�����Ǐo��									*/
#define	MBUS_DL_WAY_RD			0x02			/* �v���O���������ݏ����擾							*/
#define	MBUS_DL_START			0x03			/* �_�E�����[�h�J�n�w��								*/
#define	MBUS_DL_PRGDEL			0x04			/* �u���b�N�����w��									*/
#define	MBUS_DL_PRGWR			0x05			/* �v���O���������ݎw��								*/
#define	MBUS_DL_DELSND			0x06			/* �u���b�N�������w��(�������]���̂�.���������Ȃ�)		*/
#define	MBUS_DL_PRGSND			0x07			/* �v���O�������M�w��(�v���O�������M�̂�.���������Ȃ�)		*/
#define	MBUS_DL_PRGDELA			0x08			/* �u���b�N�ꊇ�����w��								*/
#define	MBUS_DL_PRGWRA			0x09			/* �v���O�����ꊇ�����ݎw��							*/
#define	MBUS_DL_END				0x0A			/* �_�E�����[�h�I���w��								*/
#define	MBUS_DL_PRGRD			0x0B			/* �v���O�����Ǐo���w��								*/
#define	MBUS_DL_REBOOT			0x0C			/* �v���O�����ċN���w��								*/
#define	MBUS_DL_SIFCHG			0x0D			/* �V���A���h�e���x�ύX								*/
#define	MBUS_DL_HIS_RD			0x0E			/* �_�E�����[�h����Ǐo��							*/
#define	MBUS_DL_HIS_WR			0x0F			/* �_�E�����[�h����������							*/
#define	MBUS_DL_CHKSUM			0x10			/* �`�F�b�N�T�����ʎ擾								*/ /* <S146> */
#define MBUS_DL_SF_IDX			3				/* �T�u�R�}���h�ʒu									*/ /* <S150> */
#define MBUS_DL_DATANUM_LEN		10				/* �v���O���������ݎw�ߎ��̃f�[�^���܂ł̃p�P�b�g��		*/ /* <S150> */
/*--------------------------------------------------------------------------------------------------*/
/*		�T�u�t�@���N�V���������T�C�Y																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_DL_ID_RD_RSPSIZE		22			/* ID���ǂݏo��						<S077>		*/
#define	MBUS_DL_SWVER_RD_RSPSIZE	14			/* �o�[�W�����Ǐo��					<S138>			*/
#define	MBUS_DL_WAY_RD_RSPSIZE		10			/* �v���O���������ݏ����擾							*/
#define	MBUS_DL_START_RSPSIZE		8			/* �_�E�����[�h�J�n�w��								*/
#define	MBUS_DL_PRGDEL_RSPSIZE		14			/* �u���b�N�����w��									*/
#define	MBUS_DL_PRGWR_RSPSIZE		16			/* �v���O���������ݎw��								*/
#define	MBUS_DL_DELSND_RSPSIZE		12			/* �u���b�N�������w��(�������]���̂�.���������Ȃ�)		*/
#define	MBUS_DL_PRGSND_RSPSIZE		14			/* �v���O�������M�w��(�v���O�������M�̂�.���������Ȃ�)		*/
#define	MBUS_DL_PRGDELA_RSPSIZE		8			/* �u���b�N�ꊇ�����w��								*/
#define	MBUS_DL_PRGWRA_RSPSIZE		8			/* �v���O�����ꊇ�����ݎw��							*/
#define	MBUS_DL_END_RSPSIZE			8			/* �_�E�����[�h�I���w��								*/
//#define	MBUS_DL_PRGRD_RSPSIZE		x			/* �v���O�����Ǐo���w��								*/
#define	MBUS_DL_REBOOT_RSPSIZE		6			/* �v���O�����ċN���w��								*/
//<S041>�@#define	MBUS_DL_SIFCHG_RSPSIZE		10			/* �V���A���h�e���x�ύX								*/
#define	MBUS_DL_SIFCHG_RSPSIZE		12			/* �V���A���h�e���x�ύX								*/
#define	MBUS_DL_HIS_RD_RSPSIZE		8			/* �_�E�����[�h����Ǐo��							*/
#define	MBUS_DL_HIS_WR_RSPSIZE		6			/* �_�E�����[�h����������							*/
#define	MBUS_DL_CHKSUM_RSPSIZE		10			/* �`�F�b�N�T�����ʎ擾								*/ /* <S146> */
#define MBUS_DL_ERROR_RSPSIZE		5			/* �G���[											*/
#define MBUS_DL_MIN_RSPSIZE			5			/* ���b�Z�[�W�ŏ��T�C�Y								*/
/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS �t�@�[���_�E�����[�h	���X�|���X													*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_OK		0
#define	MBUS_NG		-1
#define	MBUS_BUSY	1


/* <S1A3> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/*		�T���`�F�b�N�p��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHKSUM_OK		0
#define	CHKSUM_NG		-1
#define	CHKSUM_BUSY		1

#define CHKSUM_MAIN_PHASE_MASK	0xFF00							/* ���C���t�F�[�Y�p�}�X�N�p�^�[��	*/
#define CHKSUM_MAIN_PHASE_SHFT	8								/* ���C���t�F�[�Y�p�E�V�t�g��		*/
#define CHKSUM_MAIN_PHASE_ADD	0x0100							/* ���C���t�F�[�Y�p���Z�l			*/
#define CHKSUM_SUB_PHASE_MASK	0x00FF							/* �T�u�t�F�[�Y�p�}�X�N�p�^�[��		*/
#define CHKSUM_PHASE_END		0xFFFF							/* �`�F�b�N�T���G���h�p�^�[��		*/

#define CHKSUM_BLOCK_SIZE		0x400							/* 1�X�L�����ł̏����o�C�g��		*/
																/* �� 1�r�b�g�̂�ON�ɂ��邱��(0x400, 0x200, 0x100�Ȃ�)	*/
#define CHKSUM_BLOCK_MASK		(CHKSUM_BLOCK_SIZE - 1)			/* 1�u���b�N�I������p�}�X�N�p�^�[��*/
/* <<<<< <S1A3> */

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�_�E�����[�h��`�\����																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct{
	UCHAR	MpuNo;									/* �l�o�t�ԍ�									*/
	UCHAR	FlushNum;								/* �t���b�V����������							*/
	USHORT	DlSts1;									/* �_�E�����[�h�X�e�[�^�X�P						*/
														/*   Bit0�F�_�E�����[�h�Ή� 0/1-��Ή�/�Ή�		*/
														/*   Bit1�F�@��ID�R�[�h�L��	0/1-��/�L			*/
														/*   Bit2-15�F�\��̈�							*/
	USHORT	DlSts2;									/* �_�E�����[�h�X�e�[�^�X�Q						*/
														/*   Bit0 �FFlash #0	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit1 �FFlash #1	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit2 �FFlash #2	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit3 �FFlash #3	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit4 �FFlash #4	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit5 �FFlash #5	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit6 �FFlash #6    �޳�۰�� 0/1-��/�s��	*/
														/*   Bit7 �FFlash #7	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit8 �FFlash #8	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit9 �FFlash #9	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit10�FFlash #10	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit11�FFlash #11	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit12�FFlash #12	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit13�FFlash #13	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit14�FFlash #14	�޳�۰�� 0/1-��/�s��	*/
														/*   Bit15�FFlash #15	�޳�۰�� 0/1-��/�s��	*/
	USHORT	DlSts3;									/* �_�E�����[�h�X�e�[�^�X�R						*/
														/*   Bit0 �FFlash #0	��̧���ID 0/1-��/�L		*/
														/*   Bit1 �FFlash #1	��̧���ID 0/1-��/�L		*/
														/*   Bit2 �FFlash #2	��̧���ID 0/1-��/�L		*/
														/*   Bit3 �FFlash #3	��̧���ID 0/1-��/�L		*/
														/*   Bit4 �FFlash #4	��̧���ID 0/1-��/�L		*/
														/*   Bit5 �FFlash #5	��̧���ID 0/1-��/�L		*/
														/*   Bit6 �FFlash #6	��̧���ID 0/1-��/�L		*/
														/*   Bit7 �FFlash #7	��̧���ID 0/1-��/�L		*/
														/*   Bit8 �FFlash #8	��̧���ID 0/1-��/�L		*/
														/*   Bit9 �FFlash #9	��̧���ID 0/1-��/�L		*/
														/*   Bit10�FFlash #10	��̧���ID 0/1-��/�L		*/
														/*   Bit11�FFlash #11	��̧���ID 0/1-��/�L		*/
														/*   Bit12�FFlash #12	��̧���ID 0/1-��/�L		*/
														/*   Bit13�FFlash #13	��̧���ID 0/1-��/�L		*/
														/*   Bit14�FFlash #14	��̧���ID 0/1-��/�L		*/
														/*   Bit15�FFlash #15	��̧���ID 0/1-��/�L		*/
	USHORT	DlSts4;									/* �_�E�����[�h�X�e�[�^�X�S						*/
														/*   Bit0 �FFlash #0	���޲�ID  0/1-��/�L		*/
														/*   Bit1 �FFlash #1	���޲�ID  0/1-��/�L		*/
														/*   Bit2 �FFlash #2	���޲�ID  0/1-��/�L		*/
														/*   Bit3 �FFlash #3	���޲�ID  0/1-��/�L		*/
														/*   Bit4 �FFlash #4	���޲�ID  0/1-��/�L		*/
														/*   Bit5 �FFlash #5	���޲�ID  0/1-��/�L		*/
														/*   Bit6 �FFlash #6	���޲�ID  0/1-��/�L		*/
														/*   Bit7 �FFlash #7	���޲�ID  0/1-��/�L		*/
														/*   Bit8 �FFlash #8	���޲�ID  0/1-��/�L		*/
														/*   Bit9 �FFlash #9	���޲�ID  0/1-��/�L		*/
														/*   Bit10�FFlash #10	���޲�ID  0/1-��/�L		*/
														/*   Bit11�FFlash #11	���޲�ID  0/1-��/�L		*/
														/*   Bit12�FFlash #12	���޲�ID  0/1-��/�L		*/
														/*   Bit13�FFlash #13	���޲�ID  0/1-��/�L		*/
														/*   Bit14�FFlash #14	���޲�ID  0/1-��/�L		*/
														/*   Bit15�FFlash #15	���޲�ID  0/1-��/�L		*/
	USHORT	DlWay;									/* �V�X�e���_�E�����[�h���@�I��					*/
														/*   Bit0-3 �F�_�E�����[�h���@					*/
														/*   Bit4-15�F�\��̈�							*/
	USHORT	Prg1WrSize;								/* �P���b�Z�[�W������̏����݃o�C�g���w��		*/
	USHORT	DlStatus;								/* �_�E�����[�h�X�e�[�^�X							*/
														/*   Bit0 �F�_�E�����[�h���[�h 0/1-OFF/ON		*/
														/*   Bit1 �FFlash������ 	0/1-OFF/ON			*/
														/*   Bit2 �FFlash�������ݒ�	0/1-OFF/ON			*/
														/*   Bit3 �FFlash�ǂݏo����	0/1-OFF/ON			*/
														/*   Bit4-5�F�������(00/01/10/11)				*/
														/*   			�Ȃ�/busy/��������/�\��			*/
	ULONG	NowBps;										/*	�V�X�e���_�E�����[�h���݂̂a�o�r�l	<S041> */
	ULONG	BpsChkTime;									/* Bps Timeout Timer <S041>	*/
/* <S146> >>>>> */
	/* Message -> Sum check */
	BOOL	MicroSumCheckF;							/* �}�C�N���R�[�h���T���`�F�b�N�v�ۃt���O			*/
	BOOL	MirSumCheckF;							/* �~���[���T���`�F�b�N�v�ۃt���O					*/
	BOOL	RtmSumCheckF;							/* �����^�C�����T���`�F�b�N�v�ۃt���O				*/
	BOOL	BootSumCheckF;							/* �u�[�g���T���`�F�b�N�v�ۃt���O					*/
	BOOL	CheckSumRequestF;						/* �`�F�b�N�T�������J�n�˗��t���O					*/
	BOOL	CheckSumCancelF;						/* �`�F�b�N�T�������L�����Z���˗��t���O				*/
	/* Sum check -> Message */
	BOOL	CheckSumCalcF;							/* �`�F�b�N�T���������s���t���O						*/
	BOOL	CheckSumEndF;							/* �`�F�b�N�T�������I���t���O						*/
	BOOL	CheckSumResult;							/* �`�F�b�N�T����������	(TRUE:OK, FALSE:NG)			*/
	/* for Sum check */
	USHORT	CheckSumPhase;							/* �`�F�b�N�T���t�F�[�Y								*/ /*<S182>*/
/* <S1A3> >>>>> */
	USHORT	CheckSumVal;							/* �T���l�v�Z�p										*/
	UCHAR	*CheckSumMem;							/* �������|�C���^									*/
	LONG	CheckSumIdx;							/* �����ʒu											*/
/* <<<<< <S1A3> */
/* <<<<< <S146> */
} SYSDL;

extern SYSDL	SysDl;

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�}�N����`																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_DLMODE_RESET()	(SysDl.DlStatus = 0x0000)			/* �_�E�����[�h���[�hBit����	*/
#define SYSDL_DLMODE_ON()		(SysDl.DlStatus |= 0x0001)			/* �_�E�����[�h���[�hBit����	*/
#define SYSDL_DLMODE_OFF()		(SysDl.DlStatus &= 0xFFFE)
#define SYSDL_CHK_DLMODE_ON()	((SysDl.DlStatus & 0x0001) == 1)
#define SYSDL_CHK_DLMODE_OFF()	((SysDl.DlStatus & 0x0001) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_ERASE_ON()		(SysDl.DlStatus |= 0x0002)			/* Flash Erase Bit����			*/
#define SYSDL_ERASE_OFF()		(SysDl.DlStatus &= 0xFFFD)
#define SYSDL_CHK_ERASE_ON()	((SysDl.DlStatus & 0x0002) == 1)
#define SYSDL_CHK_ERASE_OFF()	((SysDl.DlStatus & 0x0002) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_WRITE_ON()		(SysDl.DlStatus |= 0x0004)			/* Flash Write Bit����			*/
#define SYSDL_WRITE_OFF()		(SysDl.DlStatus &= 0xFFFB)
#define SYSDL_CHK_WRITE_ON()	((SysDl.DlStatus & 0x0004) == 1)
#define SYSDL_CHK_WRITE_OFF()	((SysDl.DlStatus & 0x0004) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_READ_ON()			(SysDl.DlStatus |= 0x0008)			/* Flash Read Bit����			*/
#define SYSDL_READ_OFF()		(SysDl.DlStatus &= 0xFFF7)
#define SYSDL_CHK_READ_ON()		((SysDl.DlStatus & 0x0008) == 1)
#define SYSDL_CHK_READ_OFF()	((SysDl.DlStatus & 0x0008) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_CHK_FLASH_BUSY()	((SysDl.DlStatus & 0x000E) != 0)	/* Flash���쒆�m�F TRUE�œ��쒆	*/
/*--------------------------------------------------------------------------------------------------*/


/* ���Ή��i�ŏI�I�ɂ̓}�N��orAPI�H�j */
#ifdef _TEST_BOARD_		/* for MICROSSP */
#define	SYSDL_CHK_BASE_BLOCK( )			(((*(VUSHORT*)(0x08000032)) & 0x0400) != 0)
#else					/* for SS-Pro7 */
#define	SYSDL_CHK_BASE_BLOCK( )			(((*(VUSHORT*)(0xC0000064)) & 0x0400) != 0)
#endif

