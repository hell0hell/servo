/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdPrmCopy.c : �k�b�c�I�y���[�^ �p�����[�^�R�s�[�@�\										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	�@ �\ : �k�b�c�I�y���[�^����EEPROM���g�p���āA													*/
/*			���[�U�p�����[�^�̕ۑ�(������)�^����(�Ǐo��)���s���B									*/
/*																									*/
/*			1)�p�����[�^�Ǐo�� : LOAD-Command(0x61)���g�p���ăp�����[�^�̓Ǐo�����s��				*/
/*			2)�p�����[�^������ : SAVE-Command(0x62)���g�p���ăp�����[�^�̏����݂��s��				*/
/*																									*/
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
/*		Rev.1.00 : 2003.06.25  S.Tokuhisa															*/
/*		Rev.1.10 : 2008.12.16  T.Taniguchi	Change PrmDef/Eeprom for LongPrm						*/
/*		Rev.1.20 : 2013.10.24  K.Ozaki		Change for Mercury										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"
#include "LcdOp.h"
#include "LcdPdl.h"			/* Parameter Download */
#include "Information.h"	/* Information */
#include "Memory.h"			/* Trace Memory */

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define OBJ_READ( obj, idx, pdata ) ((obj)->Read( (obj), (idx), (pdata), MCHK_REG_READ, &LopMsg ))  /* <S073> */
#define OBJ_SEARCH( obj, num, pidx ) ((obj)->Search( (obj), (num), 0, (pidx), MCHK_REG_READ, &LopMsg ))

#undef MlibCnvLittle16
#define MlibCnvLittle16( x ) (x)					/* ��ŏ��� */

/* TODO: �����̎d�l��SGDV,Jupiter�ƈႤ!! �s��̂��ƂɂȂ�̂ŁA�{������NG�ł� */
#define MlibCopyByteMemory( x, y, z )		MlibCopyByteMemory( y, x, z )  

#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i�Q���Ή��j�̂�	*//* <S19B> */
#define Lpx_ChkDoubleBlock( typeid ) (0) /* �Ƃ肠����7S�Ɠ����� */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
#define Lpx_ChkDoubleBlock( typeid ) (( (typeid) & 0x00FF) == SVID_SGD7W_M3) /* TODO: M3�ȊO��SGD7W�ǉ� */
#endif/* <S19B> */

/****************************************************************************************************/
/*																									*/
/*		1. Define Const/Macro																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[ ��Ԉꗗ ]																					*/
/*																									*/
/*	------------------------------------------------------------------------------------------------*/
/*	���C�����[�h(PrmCopyV.MainMode)																	*/
/*	------------------------------------------------------------------------------------------------*/
/*	PRMCOPY_MAINMODE_MODE_SELECT					�@�\�I�� 										*/
/*	PRMCOPY_MAINMODE_MODE_EXE						�@�\���s 										*/
/*		--------------------------------------------------------------------------------------------*/
/*		�@�\���[�h(PrmCopyV.SubMode)																*/
/*		--------------------------------------------------------------------------------------------*/
/*		PRMCOPY_SUBMODE_SAVE						SAVE(Servo -> DgOp)			 					*/
/*		PRMCOPY_SUBMODE_LOAD						LOAD(DgOp -> Servo) 							*/
/*		PRMCOPY_SUBMODE_VERIFY						VERIFY(Servo <-> DgOp)							*/
/*		PRMCOPY_SUBMODE_DELETE						DELETE(DgOp)	 								*/
/*			----------------------------------------------------------------------------------------*/
/*			�e�@�\���ʎ��s�V�[�P���X(PrmCopyV.SubModeSeq)											*/
/*			----------------------------------------------------------------------------------------*/
/*			PRMCOPY_SUBMODE_SEQ_INIT				�@�\��������									*/
/*			PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT		�p�����[�^�u���b�N�I��							*/
/*			PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM			���s�J�n�m�F 									*/
/*			PRMCOPY_SUBMODE_SEQ_EXE					���s 											*/
/*				------------------------------------------------------------------------------------*/
/*				���s�V�[�P���X(PrmCopyV.ExeSeq)														*/
/*				------------------------------------------------------------------------------------*/
/*				----- SAVE(�T�[�{�p�b�N -> �f�W�I�y) or DELETE(�f�W�I�y) ---------------------------*/
/*				PRMCOPY_SAVE_SEQ_INIT				���s��������									*/
/*				PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ		�w�b�_���폜�R�}���h�v��						*/
/*				PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP		�w�b�_���폜�R�}���h�����m�F					*/
/*				PRMCOPY_SAVE_SEQ_PRM_SEND			�p�����[�^�f�[�^�`���R�}���h���M				*/
/*				PRMCOPY_SAVE_SEQ_PRM_RSP			�p�����[�^�f�[�^�`���R�}���h�����m�F			*/
/*				PRMCOPY_SAVE_SEQ_HEADER_SEND		�w�b�_���o�^�R�}���h���M						*/
/*				PRMCOPY_SAVE_SEQ_HEADER_RSP			�w�b�_���o�^�R�}���h�����m�F					*/
/*				PRMCOPY_SAVE_SEQ_COMPLETE			���s������ʕ\����								*/
/*				PRMCOPY_SAVE_SEQ_CANCEL				���f��ʕ\����									*/
/*				------------------------------------------------------------------------------------*/
/*				----- LOAD(�f�W�I�y -> �T�[�{�p�b�N) or VERIFY(�T�[�{�p�b�N <-> �f�W�I�y) ----------*/
/*				PRMCOPY_LOAD_SEQ_INIT				���s��������									*/
/*				PRMCOPY_LOAD_SEQ_RECONFIRM			LOAD���s�O�Ċm�F								*/
/*				PRMCOPY_LOAD_SEQ_PRM_READ_REQ		�p�����[�^READ�R�}���h�v��						*/
/*				PRMCOPY_LOAD_SEQ_PRM_READ_RSP		�p�����[�^READ�R�}���h�����m�F & �������s		*/
/*				PRMCOPY_LOAD_SEQ_COMPLETE			���s������ʕ\����								*/
/*				PRMCOPY_LOAD_SEQ_CANCEL				���f��ʕ\����									*/
/*				PRMCOPY_VERIFY_SEQ_MATCHED			VERIFY���� ��v��ʕ\����						*/
/*				PRMCOPY_VERIFY_SEQ_UNMATCHED		VERIFY���� �s��v��ʕ\����						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���C�����[�h(PrmCopyV.MainMode)																*/
/*--------------------------------------------------------------------------------------------------*/
#define	PRMCOPY_MAINMODE_MODE_SELECT		0	/* �@�\�I�� 										*/
#define	PRMCOPY_MAINMODE_MODE_EXE			1	/* �@�\���s 										*/
#define	PRMCOPY_MAINMODE_ERROR_BIT	   0x0100	/* �G���[�������r�b�g								*/
/*--------------------------------------------------------------------------------------------------*/
/*		�@�\���s���[�h(PrmCopyV.SubMode)															*/
/*--------------------------------------------------------------------------------------------------*/
#define	PRMCOPY_SUBMODE_SAVE				0	/* SAVE(Servo -> DgOp)			 					*/
#define	PRMCOPY_SUBMODE_LOAD				1	/* LOAD(DgOp -> Servo)			 					*/
#define	PRMCOPY_SUBMODE_VERIFY				2	/* VERIFY(�T�[�{�p�b�N <-> �f�W�I�y)				*/
#define	PRMCOPY_SUBMODE_DELETE				3	/* DELETE(�f�W�I�y) 								*/
#define	PRMCOPY_SUBMODE_MAX					4	/*													*/
/*--------------------------------------------------------------------------------------------------*/
/*		�@�\���s�V�[�P���X(PrmCopyV.SubModeSeq)														*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_SUBMODE_SEQ_INIT			0	/* �@�\��������										*/
#define PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT	1	/* �u���b�N�I�� 									*/
#define PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM		2	/* ���s�J�n�m�F 									*/
#define PRMCOPY_SUBMODE_SEQ_EXE				3	/* ���s 											*/
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X(PrmCopyV.ExeSeq)															*/
/*--------------------------------------------------------------------------------------------------*/
/* SAVE(Servo -> DgOp) or DELETE(DgOp)����								 							*/
#define	PRMCOPY_SAVE_SEQ_INIT				0	/* ���s��������										*/
#define	PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ		1	/* �w�b�_���폜�R�}���h�v��						*/
#define PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP		2	/* �w�b�_���폜�R�}���h�����m�F					*/
#define PRMCOPY_SAVE_SEQ_PRM_SEND			3	/* �p�����[�^�f�[�^�`���R�}���h���M					*/
#define PRMCOPY_SAVE_SEQ_PRM_RSP			4	/* �p�����[�^�f�[�^�`���R�}���h�����m�F				*/
#define PRMCOPY_SAVE_SEQ_HEADER_SEND		5	/* �w�b�_���o�^�R�}���h���M						*/
#define PRMCOPY_SAVE_SEQ_HEADER_RSP			6	/* �w�b�_���o�^�R�}���h�����m�F					*/
#define PRMCOPY_SAVE_SEQ_COMPLETE			7	/* ���s������ʕ\����								*/
#define PRMCOPY_SAVE_SEQ_CANCEL				8	/* ���f��ʕ\����									*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_SAVE_SEQ_PRM_READ_REQ		9	/* @@ �p�����[�^�f�[�^EEPROM�Ǐo��					*/
#define PRMCOPY_SAVE_SEQ_PRM_READ_WAIT		10	/* @@ �p�����[�^�f�[�^EEPROM�Ǐo��					*/
/*--------------------------------------------------------------------------------------------------*/
/* LOAD(DgOp -> Servo) or VERIFY(Servo <-> DgOp)����					 							*/
#define PRMCOPY_LOAD_SEQ_INIT				0	/* ���s��������										*/
#define PRMCOPY_LOAD_SEQ_RECONFIRM			1	/* ���s�O�Ċm�F										*/
#define PRMCOPY_LOAD_SEQ_PRM_READ_REQ		2	/* �p�����[�^READ�R�}���h�v��						*/
#define PRMCOPY_LOAD_SEQ_PRM_READ_RSP		3	/* �p�����[�^READ�R�}���h�����m�F					*/
#define PRMCOPY_LOAD_SEQ_COMPLETE			4	/* ���s������ʕ\����								*/
#define PRMCOPY_LOAD_SEQ_CANCEL				5	/* ���f��ʕ\����									*/
#define PRMCOPY_VERIFY_SEQ_MATCHED			6	/* VERIFY���� ��v��ʕ\����						*/
#define PRMCOPY_VERIFY_SEQ_UNMATCHED		7	/* VERIFY���� �s��v��ʕ\����						*/
#define PRMCOPY_LOAD_SEQ_STORE_REQ          8   /* @@ Parameter Store Request						*/
#define PRMCOPY_LOAD_SEQ_STORE_WAIT         9   /* @@ Parameter Store Wait							*/
#define PRMCOPY_LOAD_SEQ_FILL_REQ			10  /* @@ Fill Parameter Request						*/       
#define PRMCOPY_LOAD_SEQ_FILL_WAIT			11  /* @@ Fill Parameter Request						*/       
/* �f�W�I�y�p�����[�^�u���b�N�Ǘ����擾					 										*/
#define PRMCOPY_INFOGET_SEQ_INIT			0	/* ���s��������										*/
#define PRMCOPY_INFOGET_SEQ_MNG_READ_REQ	1	/* �p�����[�^�Ǘ����v��							*/
#define PRMCOPY_INFOGET_SEQ_MNG_READ_RSP	2	/* �p�����[�^�Ǘ����擾							*/
#define PRMCOPY_INFOGET_SEQ_COMPLETE		3	/* ���s����											*/
#define PRMCOPY_INFOGET_SEQ_CANCEL			4	/* ���s���f											*/
/* �G���[��ʕ\����																					*/
#define PRMCOPY_ERROR_NONE					0	/* �G���[�\���Ȃ�									*/
#define PRMCOPY_ERROR_NOT_AVAILABLE			1	/* ���s�s�G���[									*/
#define PRMCOPY_ERROR_NO_DATA				2	/* �I���f�[�^�Ȃ��G���[								*/
#define PRMCOPY_ERROR_PRM_UNMATCH			3	/* �p�����[�^�^���A���}�b�`�G���[					*/
/****************************************************************************************************/
/*		�f�W�I�y�ʐM�R�}���h�֌W��`																*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM�R�}���h�R�[�h																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RCVCMD_ALARM_BIT			0x80	/* �f�W�I�y ��M�R�}���h�A���[���r�b�g				*/
#define	LCD_RCVCMD_MASK					((UCHAR)(~LCD_RCVCMD_ALARM_BIT))	/* �R�}���h��M�}�X�N	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM�C���^�[�t�F�[�X�@���ڕʃT�C�Y(Byte)											*/
/*--------------------------------------------------------------------------------------------------*/
/* �w�b�_�� */
#define LCD_MSG_AXIS_SIZE				1		/* �����T�C�Y										*/
#define LCD_MSG_CMD_SIZE				1		/* �R�}���h�R�[�h�T�C�Y								*/
#define LCD_MSG_LEN_SIZE				2		/* ���M�o�b�t�@���T�C�Y								*/
/* �w�b�_���g�[�^���T�C�Y */
#define LCD_MSG_HEADER_SIZE				( LCD_MSG_AXIS_SIZE + LCD_MSG_CMD_SIZE + LCD_MSG_LEN_SIZE )
/* �\���f�[�^�� */
#define LCD_MSG_LED_SIZE				2		/* LED�X�e�[�^�X�T�C�Y								*/
#define LCD_MSG_DSP_SIZE				16		/* �\���X�e�[�^�X�T�C�Y								*/
#define LCD_MSG_CHR_SIZE				90		/* �\�������T�C�Y(18�J����*5�s)						*/
/* �\���f�[�^���g�[�^���T�C�Y */
#define LCD_MSG_DISPDATA_SIZE			( LCD_MSG_LED_SIZE + LCD_MSG_DSP_SIZE + LCD_MSG_CHR_SIZE )
/* �f�W�I�yEEPROM�R�}���h�� */
#define LCD_MSG_EEADR_SIZE				2		/* �f�W�I�yEEPROM�A�h���X�T�C�Y						*/
#define LCD_MSG_DATANUM_SIZE			2		/* �f�[�^���T�C�Y									*/
/* �`�F�b�N�T�� */
#define LCD_MSG_CHKSUM_SIZE				2		/* �`�F�b�N�T���T�C�Y								*/
#define LCD_MSG_OP_KEY_SIZE				2		/* �L�[���T�C�Y(��M���̂�)						*/
#define LCD_MSG_DATA_UNIT_SIZE			2		/* �P�f�[�^�T�C�Y(2Byte)							*/
#define LCD_MSG_DATA_GROUP_NUM			8		/* �P��̒ʐM�ő���M�ł���f�[�^��(�T���l������)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM�C���^�[�t�F�[�X�@�R�}���h�ʃT�C�Y(Byte)										*/
/*--------------------------------------------------------------------------------------------------*/
/* �ʏ�R�}���h���M��(0x60)�A�f�[�^��(�T���l������) */
#define LCD_TX_NORMAL_TOTAL_SIZE	(LCD_MSG_HEADER_SIZE + LCD_MSG_DISPDATA_SIZE)
/* �ʏ�R�}���h��M��(0x60)�A�f�[�^�� */
#define LCD_RX_NORMAL_TOTAL_SIZE	(LCD_MSG_HEADER_SIZE + LCD_MSG_OP_KEY_SIZE + LCD_MSG_CHKSUM_SIZE)
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM�C���^�[�t�F�[�X�@���ڃC���f�b�N�X												*/
/*--------------------------------------------------------------------------------------------------*/
/* ����M���ʃG���A  */
#define LCD_MSG_AXIS_IDX				0
#define LCD_MSG_CMD_IDX					( LCD_MSG_AXIS_IDX + LCD_MSG_AXIS_SIZE )
#define LCD_MSG_LEN_IDX					( LCD_MSG_CMD_IDX + LCD_MSG_CMD_SIZE )
/* ���M�f�[�^����(Servo -> DgOp)�C���f�b�N�X */
#define LCD_TX_MSG_EEADR_IDX			LCD_TX_NORMAL_TOTAL_SIZE
#define LCD_TX_MSG_DATANUM_IDX			( LCD_TX_MSG_EEADR_IDX + LCD_MSG_EEADR_SIZE )
#define LCD_TX_MSG_DATA_IDX				( LCD_TX_MSG_DATANUM_IDX + LCD_MSG_DATANUM_SIZE )
#define LCD_TX_MSG_LOADSUM_IDX			LCD_TX_MSG_DATA_IDX
/* ��M�f�[�^����(DgOp -> Servo)�C���f�b�N�X */
#define LCD_RX_MSG_DATANUM_IDX			( LCD_RX_NORMAL_TOTAL_SIZE - LCD_MSG_CHKSUM_SIZE )
#define LCD_RX_MSG_DATA_IDX				( LCD_RX_MSG_DATANUM_IDX + LCD_MSG_DATANUM_SIZE )
/****************************************************************************************************/
/*		���^�[���R�[�h��`																			*/
/****************************************************************************************************/
#define	RC_RETRY_OVER					-2		/* ���g���C�񐔃I�[�o�[�t���[						*/
#define	RC_NG							-1		/* �ُ�												*/
#define	RC_OK							0		/* ����												*/
#define	RC_PASS							1		/* �Ȃɂ���������									*/
#define	RC_COMPLETE						2		/* �S��������										*/
/****************************************************************************************************/
/*		���̑���`																					*/
/****************************************************************************************************/
#define	TIME_SECOND_2					2000	/* 2000ms = 2�b										*/
#define	TIME_500MSEC					500		/* 500ms											*/
#define	TIME_1000MSEC					1000	/* 1000ms											*/
#define	RETRY_CNT_MAX					3		/* �f�W�I�y�ʐM���g���C��MAX						*/
#define LCD_DISP_LINE_CHAR_MAX			17		/* ��ʕ\���J����MAX								*/
#define PRMCOPY_DISP_TITLE_MAX			1		/* �^�C�g���\���sMAX								*/
#define PRMCOPY_DISP_LIST_MAX			4		/* ���X�g�\���sMAX(�^�C�g���s������)				*/
#define PRMCOPY_DISP_LINE_MAX			(PRMCOPY_DISP_TITLE_MAX + PRMCOPY_DISP_LIST_MAX) /*�\���sMAX*/
#define PRMCOPY_VERIFY_ERR_DISP_TBL_MAX	20		/* VERIFY�s��v�G���[���X�g�e�[�u������MAX(5���)	*/
#define	PRMCOPY_BLOCK_WORD_MAX			512		/* �f�W�I�yEEPROM�̃p�����[�^�P�u���b�N�̃��[�h��	*/
/* �f�W�I�yEEPROM �p�����[�^�Ǘ���� */
#define	PRMCOPY_MNG_INFO_TOP0			0xE00	/* �̈�O�F�擪�A�h���X   (Word Addr)				*/
#define	PRMCOPY_MNG_INFO_SIZ0			0x003	/* �̈�O�F�Ǘ����T�C�Y (Word Size)				*/
#define	PRMCOPY_MNG_INFO_TOP1			0xE20	/* �̈�P�F�擪�A�h���X   (Word Addr)				*/
#define	PRMCOPY_MNG_INFO_SIZ1			0x010	/* �̈�P�F�Ǘ����T�C�Y (Word Size)				*/
#define	PRMCOPY_MNG_INFO_BLOCK_MAX		7		/* �p�����[�^�u���b�N��								*/
#define	PRMCOPY_MNG_DATA_NONE			((USHORT)0xFFFF)	/* �p�����[�^�Ǘ��e�[�u���ݒ�Ȃ��f�[�^	*/
/* �p�����[�^�Ǘ���񏈗��^�C�v */
#define	PRMCOPY_MNG_INFODATA_CLEAR		0		/* �N���A											*/
#define	PRMCOPY_MNG_INFODATA_SET		1		/* �f�[�^�Z�b�g										*/
#define	PRMCOPY_MNG_INFODATA_RECIEVE	2		/* ��M�f�[�^�擾									*/
#define	LCD_DISP_LINE_1					0		/* LCD�f�W�I�y �\���s �P							*/
#define	LCD_DISP_LINE_2					1		/* LCD�f�W�I�y �\���s �Q							*/
#define	LCD_DISP_LINE_3					2		/* LCD�f�W�I�y �\���s �R							*/
#define	LCD_DISP_LINE_4					3		/* LCD�f�W�I�y �\���s �S							*/
#define	LCD_DISP_LINE_5					4		/* LCD�f�W�I�y �\���s �T							*/
/****************************************************************************************************/
/*		�}�N����`																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�yEEPROM�A�h���X�Z�o : �p�����[�^�u���b�NNO�ƃp�����[�^IndexNo���Z�o				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_PRM_ADDR_GET(a,b)		(USHORT)((a * PRMCOPY_BLOCK_WORD_MAX) + b)
/*--------------------------------------------------------------------------------------------------*/
/*		��M�o�b�t�@�F�R�}���h�R�[�h�擾															*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_CMD_CODE_GET( )			( LopMsg.CmdBuf[LCD_MSG_CMD_IDX] )
/*--------------------------------------------------------------------------------------------------*/
/*		��M�o�b�t�@�F��M�f�[�^�擾																*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_CMD_DATA_GET( a )		MlibCnvLittle16( \
		*(USHORT *)&LopMsg.CmdBuf[ LCD_RX_MSG_DATA_IDX + (a * LCD_MSG_DATA_UNIT_SIZE) ] )
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		2. LCD Operator Message I/F																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�⑫�F#define�����G�Ȃ̂ŁA�\���̒�`+#define�ɕύX����B(�����p)							*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM�R�}���h�R�[�h																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_CMD_ALMBIT		0x80				/* �f�W�I�y ��M�R�}���h�A���[���r�b�g				*/
#define	LCD_CMD_ALMMSK		(~LCD_CMD_ALMBIT)	/* �f�W�I�y ��M�R�}���h�A���[���}�X�N				*/
/*--------------------------------------------------------------------------------------------------*/
/*		���M�o�b�t�@�f�[�^�\����`(���ݎg�p��)														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x62									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h(0x61�`0x64)						*/
		USHORT	Length;							/* ���M���b�Z�[�W��									*/
		USHORT	LedSts;							/* �k�d�c�\�����									*/
		UCHAR	DispSts[16];					/* �\���`��											*/
		UCHAR	DispLine[PRMCOPY_DISP_LINE_MAX][LCD_DISP_LINE_CHAR_MAX+1];	/* ��ʕ\�����			*/
		USHORT	EepAddr;						/* �f�W�I�yEEPROM�A�h���X(WORD)						*/
		USHORT	DataNum;						/* ����M�f�[�^��(0�`8)								*/
		USHORT	Data[LCD_MSG_DATA_GROUP_NUM];	/* ����M�f�[�^										*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	DispLineBlink[PRMCOPY_DISP_LINE_MAX];		/* Work : ��ʕ\���u�����N���			*/
} PRM_COPY_TXBUF;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		3. Structure Definition for Variables														*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�R�s�[�Ǘ��p�p�����[�^															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	TypeID;							/* PrmCopy�f�[�^���i����ID							*/
		ADJUST_STRUCT_ALIGN( Dummy )			/* Adjust HEW Structure to Long Align				*/
} PRM_COPY_MNGP;
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�R�s�[�@�\��Ɨp�ϐ�																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	MainMode;						/* ���C�����[�h(�@�\�I�� or �@�\���s)				*/
		USHORT	SubMode;						/* �@�\���[�h(SAVE �` DELETE)						*/
		USHORT	SubModeSeq;						/* �e�@�\���ʎ��s�V�[�P���X(�@�\�������� �` ���s)	*/
		USHORT	ExeSeq;							/* ���s�V�[�P���X(�@�\��)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	BlockNo;						/* �p�����[�^�u���b�NNO								*/
		USHORT	ErrorNo;						/* �G���[���b�Z�[�W���NO							*/
		USHORT	RetryCnt;						/* �f�W�I�y�ʐM���g���C��							*/
//		USHORT	UserChkSum;						/* ���[�U�[�`�F�b�N�T���v�Z�p						*/
		USHORT	BlockMngAreaNo;					/* �u���b�N�Ǘ����G���A�ԍ�(�A�N�Z�X�Ǘ��p)		*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	DispHoldTime;					/* �Q�b�ԉ�ʕ\���^�C�}�[							*/
		ULONG	DispKeepTime;					/* �\���Œ�p�^�C�}									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	PrmReadIndex;					/* �p�����[�^Read�C���f�b�N�X						*/
		USHORT	PrmReadIndex_Last;				/* �O��p�����[�^Read�C���f�b�N�X					*/
		USHORT	PrmWriteIndex;					/* �p�����[�^Write�C���f�b�N�X						*/
		USHORT	PrmWriteIndex_Last;				/* �O��p�����[�^Write�C���f�b�N�X					*/
	/*----------------------------------------------------------------------------------------------*/
//		USHORT	PrmIndex;						/* �p�����[�^�����C���f�b�N�X(Bit0:LongL/H Flag)	*/
//		USHORT	PrmIndex_Last;					/* �O��p�����[�^�����C���f�b�N�X					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	VerifyErrListNo;				/* VERIFY�s��v���X�gNO								*/
		USHORT	VerifyErrTblCnt;				/* VERIFY�s��v���X�g�e�[�u���g�p����				*/
		USHORT	VerifyErrTotalCnt;				/* VERIFY�s��v��������								*/
		USHORT	VerifyErrPrmIdxTbl[PRMCOPY_VERIFY_ERR_DISP_TBL_MAX]; /* VERIFY�s��v���X�g�e�[�u��	*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	*TraceBuffer;					/* @@ Pointer the Work Buffer						*/
		USHORT	VerifyNumber;					/* @@ Number of Parameters for Verify 				*/
		USHORT	Reserved1;						/* @@												*/
} PRM_COPY_INFO;
/*--------------------------------------------------------------------------------------------------*/
/*		�k�b�c�I�y���[�^�����d�d�o�q�n�l�̃p�����[�^�u���b�N�Ǘ����								*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	ServoID;						/* �T�[�{�p�b�N���i�h�c								*/
		USHORT	AmpWatt;						/* �T�[�{�p�b�N�e��[Watt]							*/
		USHORT	AmpVolt;						/* �T�[�{�p�b�N�o�͓d���R�[�h						*/
		USHORT	FormatID;						/* �p�����[�^�t�H�[�}�b�g�h�c�@						*/
} PRM_MNGB_INFO;
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	SvPrm;							/* �T�[�{���[�U�萔�o�^��							*/
//		USHORT	UsrPrm;							/* �I�v�V�������[�U�萔�o�^��						*/
} PRM_ENTNUM_INFO;
/*--------------------------------------------------------------------------------------------------*/
/*		PRM_COPY_INFO.PrmIndex Operation Macro														*/
/*--------------------------------------------------------------------------------------------------*/
/*		PrmIndex :   0 bit : LongPrm LowWord/HighWord Flag											*/
/*		PrmIndex : F-1 bit : All Parameter Manage Table PnPrmTbl[] Index							*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_HiWdFlag( Idx )		((Idx) & 0x01)
//#define LPX_GET_PrmDefPtr( Idx )	(PnPrmTbl[((Idx)>>1)].PrmDefPtr)
#define LPX_INC_SrhIndex( Idx )		((Idx) += 2)
//#define	LPX_CHK_VarIsLong( Idx )	(PnPrmTbl[((Idx)>>1)].PrmDefPtr->Attr.VarIsLong)
//#define LPX_INC_PrmIndex( Idx )		if( LPX_CHK_VarIsLong( Idx ) ){ (Idx) += 1;}else{ (Idx) += 2;}
/*--------------------------------------------------------------------------------------------------*/
/*		Long Parameter Operation Macro for Big/Little Endian										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Macro Argument     :   lwd    hwd															*/
/*		LongPrm Word Align : {LoNum, HiNum} : ULONGOF_REGDATA( (hwd<<16) + lwd ) <-- SGDV			*/
/*		LongPrm Word Align : {HiNum, LoNum} : ULONGOF_REGDATA( (lwd<<16) + hwd ) <-- ????			*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_BIG_ENDIAN == 1
#define GET_LWD_EEPDATA( xlong )	(USHORT)((xlong)>>0 )
#define GET_HWD_EEPDATA( xlong )	(USHORT)((xlong)>>16)
#define ULONGOF_EEPDATA( lwd, hwd )	(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#else
#define GET_LWD_EEPDATA( xlong )	(USHORT)((xlong)>>0 )
#define GET_HWD_EEPDATA( xlong )	(USHORT)((xlong)>>16)
#define ULONGOF_EEPDATA( lwd, hwd )	(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Check EEPROM Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_CHK_EEP_PRM( pDef )		( (pDef->Attr.Place == PRMDEF_RAM_EEPROM) && \
									  (pDef->RamPtr != NULL) && (pDef->Attr.AliasPrm == FALSE) )
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		5. Const Data Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�@��h�c�e�[�u��(�\�������񌟍��e�[�u��)												*/
/*--------------------------------------------------------------------------------------------------*/
static	const	SVIDTBL			SvIdTbl[] = { SVIDTBL_ENTRY_DEF };
#define	SVIDTBL_ENTRY_NUM		(sizeof(SvIdTbl)/sizeof(SvIdTbl[0]))
/*--------------------------------------------------------------------------------------------------*/
/*		�A���v�e�ʕ\�������񌟍��e�[�u��															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct
{
	USHORT 	AmpVolt;
	USHORT	AmpWatt;
    UCHAR 	DispStr[10];
	UCHAR	DispStrW[10];	
} AMP_WATT_TBL;
/*--------------------------------------------------------------------------------------------------*/
const	AMP_WATT_TBL	AmpWattTbl[] = {
	{	0,	30,		"1R0",	"A3"	},		/*   30 W		100V	*/
/*--------------------------------------------------------------------------------------------------*/
	{	1,	50,		"R70",	"A5"	},		/*   50 W		200V	*/
	{	1,	100,	"R90",	"01"	},		/*  100 W				*/
	{	1,	200,	"1R6",	"02"	},		/*  200 W				*/
	{	1,	400,	"2R8",	"04"	},		/*  400 W				*/
	{	1,	500,	"3R8",	"05"	},		/*  500 W				*/
	{	1,	750,	"5R5",	"08"	},		/*  750 W				*/
	{	1,	1000,	"7R6",	"10"	},		/*  1.0 kW				*/
	{	1,	1500,	"120",	"15"	},		/*  1.5 kW				*/
	{	1,	2000,	"180",	"20"	},		/*  2.0 kW				*/
	{	1,	3000,	"200",	"30"	},		/*  3.0 kW				*/
	{	1,	5000,	"330",	"50"	},		/*  5.0 kW				*/
	{	1,	6000,	"470",	"60"	},		/*  6.0 kW				*/
	{	1,	7500,	"550",	"75"	},		/*  7.5 kW				*/
	{	1,	11000,	"590",	"1A"	},		/* 11.0 kW				*/
	{	1,	15000,	"780",	"1E"	},		/* 15.0 kW				*/
/*--------------------------------------------------------------------------------------------------*/
	{	2,	500,	"1R9",	"05"	},		/*  500 W		400V	*/
	{	2,	1000,	"3R5",	"10"	},		/*  1.0 kW				*/
	{	2,	1500,	"5R4",	"15"	},		/*  1.5 kW				*/
	{	2,	2000,	"8R4",	"20"	},		/*  2.0 kW				*/
	{	2,	3000,	"120",	"30"	},		/*  3.0 kW				*/
	{	2,	5000,	"170",	"50"	},		/*  5.0 kW				*/
	{	2,	6000,	"210",	"60"	},		/*  6.0 kW				*/
	{	2,	7500,	"250",	"75"	},		/*  7.5 kW				*/
	{	2,	11000,	"280",	"1A"	},		/* 11.0 kW				*/
	{	2,	15000,	"370",	"1E"	},		/* 15.0 kW				*/
/*--------------------------------------------------------------------------------------------------*/
	{	5,	50,		"R70",	"A5"	},		/*   50 W		�{�d��	*/
	{	5,	100,	"R90",	"01"	},		/*  100 W				*/
	{	5,	200,	"1R6",	"02"	},		/*  200 W				*/
	{	5,	400,	"2R8",	"04"	},		/*  400 W				*/
};
/*--------------------------------------------------------------------------------------------------*/
#define	AMP_WATT_TBL_IDX_MAX	(sizeof(AmpWattTbl)/sizeof(AMP_WATT_TBL))
/*--------------------------------------------------------------------------------------------------*/
/*		���͓d���\�������񌟍��e�[�u��																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct
{
	USHORT 	Code;
	UCHAR 	DispStr[10];
} AMP_VOLT_TBL;
/*--------------------------------------------------------------------------------------------------*/
const	AMP_VOLT_TBL	AmpVoltTbl[] =
{
	{	0,		"B"		},		/* AC 100V 				*/
	{	1,		"A"		},		/* AC 200V 				*/
	{	2,		"D"		},		/* AC 400V 				*/
	{	5,		"F"		},		/* �{�d��(AC 100V x 2)	*/
};
/*--------------------------------------------------------------------------------------------------*/
#define	AMP_VOLT_TBL_IDX_MAX	(sizeof(AmpVoltTbl)/sizeof(AMP_VOLT_TBL))
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		5. Const String Data Definition																*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		��ʃ^�C�g��(��ʉE��ɕ\������郂�[�h�ʂ̃^�C�g��)										*/
/****************************************************************************************************/
#if 0		/* <S154> */
const UCHAR PrmCopyDispTitleTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"           -COPY-",			/* [00] : ���C���^�C�g��	PRMCOPY_DISPTITLE_MAIN				*/
	"       -SERVO\176OP-",			/* [01] : SAVE�^�C�g��		PRMCOPY_DISPTITLE_SAVE				*/
	"       -OP\176SERVO-",			/* [02] : LOAD�^�C�g��		PRMCOPY_DISPTITLE_LOAD				*/
	"         -VERIFY-",			/* [03] : VERIFY�^�C�g��	PRMCOPY_DISPTITLE_VERIFY			*/
	"           -LIST-",			/* [04] : DELETE�^�C�g��	PRMCOPY_DISPTITLE_DELETE			*/
};
#else		/* <S154> */
const UCHAR PrmCopyDispTitleTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"             COPY",			/* [00] : ���C���^�C�g��	PRMCOPY_DISPTITLE_MAIN				*/
	"         SERVO\176OP",			/* [01] : SAVE�^�C�g��		PRMCOPY_DISPTITLE_SAVE				*/
	"         OP\176SERVO",			/* [02] : LOAD�^�C�g��		PRMCOPY_DISPTITLE_LOAD				*/
	"           VERIFY",			/* [03] : VERIFY�^�C�g��	PRMCOPY_DISPTITLE_VERIFY			*/
	"             LIST",			/* [04] : DELETE�^�C�g��	PRMCOPY_DISPTITLE_DELETE			*/
};
#endif
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispTitleTblIndex		/* �^�C�g���\��������e�[�u���@�C���f�b�N�X�m�n��`				*/
{
	PRMCOPY_DISPTITLE_MAIN = 0,
	PRMCOPY_DISPTITLE_SAVE,
	PRMCOPY_DISPTITLE_LOAD,
	PRMCOPY_DISPTITLE_VERIFY,
	PRMCOPY_DISPTITLE_DELETE,
};
/****************************************************************************************************/
/*		�@�\�I�����(�@�\�ꗗ)																		*/
/****************************************************************************************************/
const UCHAR PrmCopyDispModeStrTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"1:SERVO\176OP",				/* [00] : SAVE 				PRMCOPY_DISPMODE_SAVE				*/
	"2:OP\176SERVO",				/* [01] : LOAD				PRMCOPY_DISPMODE_LOAD				*/
	"3:VERIFY",						/* [02] : VERIFY			PRMCOPY_DISPMODE_VERIFY				*/
	"4:LIST",						/* [03] : DELETE			PRMCOPY_DISPMODE_DELETE				*/
};
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispModeTblIndex		/* �@�\�ꗗ�\��������e�[�u���@�C���f�b�N�X�m�n��`				*/
{
	PRMCOPY_DISPMODE_SAVE = 0,
	PRMCOPY_DISPMODE_LOAD,
	PRMCOPY_DISPMODE_VERIFY,
	PRMCOPY_DISPMODE_DELETE,
};
/****************************************************************************************************/
/*		�p�����[�^�u���b�N�I�����(�\���ԍ�)														*/
/****************************************************************************************************/
const UCHAR PrmCopyDispBlockStrTbl[][4] =
{
	"00:",							/* [00] : PRMCOPY_DISPBLOCK_NO_00								*/
	"01:",							/* [01] : PRMCOPY_DISPBLOCK_NO_01								*/
	"02:",							/* [02] : PRMCOPY_DISPBLOCK_NO_02								*/
	"03:",							/* [03] : PRMCOPY_DISPBLOCK_NO_03								*/
	"04:",							/* [04] : PRMCOPY_DISPBLOCK_NO_04								*/
	"05:",							/* [05] : PRMCOPY_DISPBLOCK_NO_05								*/
	"06:",							/* [06] : PRMCOPY_DISPBLOCK_NO_06								*/
};
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispBlockTblIndex		/* �p�����[�^�u���b�N�\��No������e�[�u���@�C���f�b�N�XNo��`	*/
{
	PRMCOPY_DISPBLOCK_NO_00 = 0,
	PRMCOPY_DISPBLOCK_NO_01,
	PRMCOPY_DISPBLOCK_NO_02,
	PRMCOPY_DISPBLOCK_NO_03,
	PRMCOPY_DISPBLOCK_NO_04,
	PRMCOPY_DISPBLOCK_NO_05,
	PRMCOPY_DISPBLOCK_NO_06,
};
/****************************************************************************************************/
/*		���̑��\��������																			*/
/****************************************************************************************************/
const UCHAR PrmCopyDispStrTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"                 ",			/* [00]:PRMCOPY_DISPSTR_SPACE 				*/
	"  Start :[WRITE]",				/* [01]:PRMCOPY_DISPSTR_START_WRITE			*/
	"  Start :[READ]",				/* [02]:PRMCOPY_DISPSTR_START_READ			*/
	"  Return:[MODE]",				/* [03]:PRMCOPY_DISPSTR_RETURN_MODE			*/
	"   Reading",					/* [04]:PRMCOPY_DISPSTR_READING				*/
	"   Writing",					/* [05]:PRMCOPY_DISPSTR_WRITING				*/
	"   Verifying",					/* [06]:PRMCOPY_DISPSTR_VERIFYING			*/
	"   Deleting",					/* [07]:PRMCOPY_DISPSTR_DELETING			*/
	"    Parameters..",				/* [08]:PRMCOPY_DISPSTR_PARAMETERS_DOT		*/
	"   Completed",					/* [09]:PRMCOPY_DISPSTR_COMPLETED			*/
	"    Canceled",					/* [10]:PRMCOPY_DISPSTR_CANCELED			*/
	"   Unmatched",					/* [11]:PRMCOPY_DISPSTR_UNMATCHED_RIGHT		*/
	"    Parameters",				/* [12]:PRMCOPY_DISPSTR_PARAMETERS_RIGHT	*/
	"   No Data",					/* [13]:PRMCOPY_DISPSTR_NO_DATA				*/
	"  Not Available",				/* [14]:PRMCOPY_DISPSTR_NOT_AVAILABLE		*/
	"****",							/* [15]:PRMCOPY_DISPSTR_ASTER				*/
	"-",							/* [16]:PRMCOPY_DISPSTR_HIFUN				*/
	" All Parameters",				/* [17]:PRMCOPY_DISPSTR_ALL_PARAMETERS		*/
	"  are matched",				/* [18]:PRMCOPY_DISPSTR_ARE_MATCHED			*/
	"Unmatched",					/* [19]:PRMCOPY_DISPSTR_UNMATCHED_LEFT		*/
	"Parameters",					/* [20]:PRMCOPY_DISPSTR_PARAMETERS_LEFT		*/
	"<<SERVO>>",					/* [21]:PRMCOPY_DISPSTR_SERVO				*/
	"<< OP >>",						/* [22]:PRMCOPY_DISPSTR_OP					*/
	"<<OP-SERVO>>",					/* [23]:PRMCOPY_DISPSTR_OP_SERVO			*/
	"??",							/* [24]:PRMCOPY_DISPSTR_QUESTION_2			*/
	"A",							/* [25]:PRMCOPY_DISPSTR_A					*/
	"F",							/* [26]:PRMCOPY_DISPSTR_F					*/
	"?",							/* [27]:PRMCOPY_DISPSTR_QUESTION_1			*/
	"  FILE DELETE    ",			/* [28]:PRMCOPY_DISPSTR_FILE_DELETE			*/
	"Pn",							/* [29]:PRMCOPY_DISPSTR_PN					*/
	"To Many Errors",				/* [30]:PRMCOPY_DISPSTR_TO_MANY_ERRORS		*/
	"File List Loading",			/* [31]:PRMCOPY_DISPSTR_FILE_LIST_LOADING	*/
	"  Please Wait...",				/* [32]:PRMCOPY_DISPSTR_PLEASE_WAIT			*/
	"  Reconfirm",					/* [33]:PRMCOPY_DISPSTR_RECONFIRM			*/
	"   OK :[DATA]",				/* [34]:PRMCOPY_DISPSTR_OK_DATA				*/
	"   NO :[MODE]",				/* [35]:PRMCOPY_DISPSTR_NO_MODE				*/
	"????",							/* [36]:PRMCOPY_DISPSTR_QUESTION_4			*/
	"???",							/* [37]:PRMCOPY_DISPSTR_QUESTION_3			*/
};
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispStrTblIndex			/* �\��������e�[�u���@�C���f�b�N�X�m�n��`	*/
{
	PRMCOPY_DISPSTR_SPACE = 0,
	PRMCOPY_DISPSTR_START_WRITE,
	PRMCOPY_DISPSTR_START_READ,
	PRMCOPY_DISPSTR_RETURN_MODE,
	PRMCOPY_DISPSTR_READING,
	PRMCOPY_DISPSTR_WRITING,
	PRMCOPY_DISPSTR_VERIFYING,
	PRMCOPY_DISPSTR_DELETING,
	PRMCOPY_DISPSTR_PARAMETERS_DOT,
	PRMCOPY_DISPSTR_COMPLETED,
	PRMCOPY_DISPSTR_CANCELED,
	PRMCOPY_DISPSTR_UNMATCHED_RIGHT,
	PRMCOPY_DISPSTR_PARAMETERS_RIGHT,
	PRMCOPY_DISPSTR_NO_DATA,
	PRMCOPY_DISPSTR_NOT_AVAILABLE,
	PRMCOPY_DISPSTR_ASTER,
	PRMCOPY_DISPSTR_HIFUN,
	PRMCOPY_DISPSTR_ALL_PARAMETERS,
	PRMCOPY_DISPSTR_ARE_MATCHED,
	PRMCOPY_DISPSTR_UNMATCHED_LEFT,
	PRMCOPY_DISPSTR_PARAMETERS_LEFT,
	PRMCOPY_DISPSTR_SERVO,
	PRMCOPY_DISPSTR_OP,
	PRMCOPY_DISPSTR_OP_SERVO,
	PRMCOPY_DISPSTR_QUESTION_2,
	PRMCOPY_DISPSTR_A,
	PRMCOPY_DISPSTR_F,
	PRMCOPY_DISPSTR_QUESTION_1,
	PRMCOPY_DISPSTR_FILE_DELETE,
	PRMCOPY_DISPSTR_PN,
	PRMCOPY_DISPSTR_TO_MANY_ERRORS,
	PRMCOPY_DISPSTR_FILE_LIST_LOADING,
	PRMCOPY_DISPSTR_PLEASE_WAIT,
	PRMCOPY_DISPSTR_RECONFIRM,
	PRMCOPY_DISPSTR_OK_DATA,
	PRMCOPY_DISPSTR_NO_MODE,
	PRMCOPY_DISPSTR_QUESTION_4,
	PRMCOPY_DISPSTR_QUESTION_3,
};
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		6. Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
//#pragma section sidata begin
PRM_COPY_MNGP	PrmCopyP;
PRM_COPY_INFO	PrmCopyV;
PRM_MNGB_INFO	PrmMngTbl[PRMCOPY_MNG_INFO_BLOCK_MAX];
PRM_ENTNUM_INFO	PrmEntNum;
extern MSGIF	LopMsg;							/* �k�b�c�I�y���[�^�F���b�Z�[�W�h�e�f�[�^			*/
//#pragma section sidata end
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		7. Functions Prototype List																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[ �֐��\�� ]																					*/
/*																									*/
/*	RpxLcdPrmCopyMain()							�p�����[�^�R�s�[���C��								*/
/*	|																								*/
/*	|  PrmCopyInit()							��������											*/
/*	|  |  PrmCopyCmdInit()						�f�W�I�y�ʐM��������								*/
/*	|  |  PrmCopyNormalCmdSet()					�f�t�H���g�Œʏ�ʐM�R�}���h�Z�b�g					*/
/*	|  |  |																							*/
/*	|  PrmCopyOpMain()							�L�[���상�C������									*/
/*	|  |  PrmCopyModeSelect()					�@�\�I��											*/
/*	|  |  |  RpxLcdPrmCopyInit() 				����������(�\���̕ϐ��S�N���A���A����̂�)			*/
/*	|  |  |  PrmCopyInit()						��������(�ʐM�o�b�t�@�N���A���A����)				*/
/*	|  |  |  PrmCopyModeSelectCheck()			�@�\�I���`�F�b�N����			 					*/
/*	|  |  PrmCopyModeExe()						�@�\���s											*/
/*	|  |  |  PrmCopyBlockDispListMake() 		�u���b�N�ꗗ�\���e�[�u���쐬����					*/
/*	|  |  |  PrmCopyBlockSelect()				�u���b�N�I������				 					*/
/*	|  |  |  |	PrmCopyBlockSelectCheck()		�u���b�N�I���`�F�b�N����							*/
/*	|  |  |  |	PrmCopyBlockTypeCheck()			�u���b�N�^�C�v�`�F�b�N����							*/
/*	|  |  |  PrmCopyExeConfirm()				���s�J�n�m�F����									*/
/*	|  |  |  PrmCopySaveKeyOp()					SAVE(Servo -> DgOp)�� Key����						*/
/*	|  |  |  PrmCopySaveOrDeleteExe()			SAVE OR DELETE(DgOp)���s							*/
/*	|  |  |  |	PrmCopyPrmMngTblOp()			�f�W�I�y �p�����[�^�u���b�N�Ǘ����ҏW����			*/
/*	|  |  |  |	PrmCopySaveCmdSet()				�ʐM��Ɨ̈��SAVE�R�}���h�Z�b�g					*/
/*	|  |  |  |	PrmCopyCmdRspCheck()			�f�W�I�y�ʐM�R�}���h�����m�F����					*/
/*	|  |  |  |	PrmCopyUsrPrmSearch()			���[�U�[�p�����[�^����								*/
/*	|  |  |  |	KpiRstLongTimer()				�^�C�}�[���Z�b�g									*/
/*	|  |  |  |	KpiGetLongTimer()				�^�C�}�[�l�擾										*/
/*	|  |  |  PrmCopyLoadKeyOp()					LOAD(DgOp -> Servo)�� Key����						*/
/*	|  |  |  PrmCopyVerifyKeyOp()				VERIFY(Servo <-> DgOp)�� Key����					*/
/*	|  |  |  PrmCopyLoadOrVerifyExe()			LOAD OR VERIFY���s									*/
/*	|  |  |  |  PrmCopyLoadCmdSet()				�ʐM��Ɨ̈��LOAD�R�}���h�Z�b�g					*/
/*	|  |  |  |  PrmCopyCmdRspCheck()			�f�W�I�y�ʐM�R�}���h�����m�F����					*/
/*	|  |  |  |  PrmCopyLoadExePrmWrite()		�P���M��(�ő�W��)�̃p�����[�^��					*/
/*	|  |  |  |  |									�T�[�{�p�b�N��EEPROM�֏������ޏ���				*/
/*	|  |  |  |  PrmCopyVerifyExePrmVerify()		�P���M��(�ő�W��)�̃p�����[�^��					*/
/*	|  |  |  |  |									�T�[�{�p�b�N�̃p�����[�^(EEPROM)��VERIFY		*/
/*	|  |  |  |  |  PrmCopyUsrPrmSearch()		���[�U�[�p�����[�^Index�擾							*/
/*	|  |  |  |  KpiRstLongTimer()				�^�C�}�[���Z�b�g									*/
/*	|  |  |  |  KpiGetLongTimer()				�^�C�}�[�l�擾										*/
/*	|  |  PrmCopyErrorOp()						�G���[����������									*/
/*	|  |  |																							*/
/*	|  PrmCopyDispMain()						��ʕ\���f�[�^�쐬���C������						*/
/*	|  |  PrmCopyDispModeSelect()				�@�\�I����ʁ@						�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispBlockSelect()				�u���b�N�I����ʁ@					�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispExeConfirm()				���s�J�n�m�F��ʁ@					�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispSaveExe()					SAVE(Servo -> DgOp)���s�����		�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispLoadExe()					LOAD(DgOp -> Servo)���s�����		�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispVerifyExe()				VERIFY(Servo <-> DgOp)���s�����	�\���f�[�^�쐬	*/
/*	|  |  |  PrmCopyDispVerifyUnmatchList()		VERIFY���� �s��v��ʁ@				�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispDeleteExe()				DELETE���s����ʁ@					�\���f�[�^�쐬	*/
/*	|  |  PrmCopyDispError()					�G���[���b�Z�[�W��ʁ@				�\���f�[�^�쐬	*/
/*	|  |  |																							*/
/*	|  PrmCopySetSendBuf()						�ʐM��Ɨ̈悩��A���M�o�b�t�@�փf�[�^�Z�b�g		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[ �֐��ꗗ ]																					*/
/*																									*/
/*		-------------------------------------------------------------------------------------------	*/
/*		RpxLcdPrmCopyMain()				�p�����[�^�R�s�[�@�\���C��									*/
/*		-------------------------------------------------------------------------------------------	*/
/*		PrmCopyOpInit()					��������													*/
/*		PrmCopyOpMain()					�L�[���상�C������											*/
/*		PrmCopyModeSelect()				�@�\�I��													*/
/*		PrmCopyModeExe()				�@�\���s													*/
/*		PrmCopyBlockSelect()			�p�����[�^�u���b�N�I��										*/
/*		PrmCopyBlockSelectCheck()		�u���b�N�I���`�F�b�N����									*/
/*		PrmCopyBlockTypeCheck()			�u���b�N�^�C�v�`�F�b�N����									*/
/*		PrmCopyExeConfirm()				���s�J�n�m�F����											*/
/*		PrmCopySaveOrDeleteExe()		SAVE(Servo -> DgOp) or DELETE(DgOp)���s						*/
/*		PrmCopyLoadOrVerifyExe()		LOAD(DgOp -> Servo)���s										*/
/*		PrmCopySaveKeyOp()				SAVE(Servo -> DgOp)��  Key����								*/
/*		PrmCopyLoadKeyOp()				LOAD(DgOp -> Servo)��  Key����								*/
/*		PrmCopyVerifyKeyOp()			VERIFY(Servo <-> DgOp)��  Key����							*/
/*		PrmCopyLoadExePrmWrite()		�P���M��(�ő�W��)�̃p�����[�^��							*/
/*												�T�[�{�p�b�N��EEPROM�֏�������						*/
/*		PrmCopyVerifyExePrmVerify()		�P���M��(�ő�W�̃p�����[�^��							*/
/*												�T�[�{�p�b�N�̃p�����[�^(EEPROM)�Ƃ�VERIFY			*/
/*		PrmCopyModeSelectCheck()		�@�\�I���`�F�b�N����										*/
/*		PrmCopyErrorOp()				�G���[����������											*/
/*		PrmCopyUsrPrmSearch()			���[�U�[�p�����[�^��������									*/
/*		ConvertHexToStr()				HEX���當����ւ̕ϊ�����									*/
/*		-------------------------------------------------------------------------------------------	*/
/*		PrmCopyDispMain()				��ʕ\���f�[�^�쐬�������C��								*/
/*		PrmCopyDispError()				�G���[���b�Z�[�W��ʁ@				�\���f�[�^�쐬			*/
/*		PrmCopyDispModeSelect()			�@�\�I����ʁ@						�\���f�[�^�쐬			*/
/*		PrmCopyDispBlockSelect()		�u���b�N�I����ʁ@					�\���f�[�^�쐬			*/
/*		PrmCopyDispExeConfirm()			���s�J�n�m�F��ʁ@					�\���f�[�^�쐬			*/
/*		PrmCopyDispSaveExe()			SAVE(Servo -> DgOp)���s����ʁ@		�\���f�[�^�쐬			*/
/*		PrmCopyDispLoadExe()			LOAD(DgOp -> Servo)���s����ʁ@		�\���f�[�^�쐬			*/
/*		PrmCopyDispVerifyExe()			VERIFY(Servo <-> DgOp)���s�����	�\���f�[�^�쐬			*/
/*		PrmCopyDispVerifyUnmatchList()	VERIFY���� �s��v���				�\���f�[�^�쐬			*/
/*		PrmCopyDispDeleteExe()			DELETE(DgOp)���s�����				�\���f�[�^�쐬			*/
/*		PrmCopyDispBlockHeader()		�u���b�N�I������					�w�b�_�[�쐬			*/
/*		PrmCopyMakeBlockDispLine()		�u���b�N�\���f�[�^�쐬����			�\���f�[�^�쐬			*/
/*		PrmCopyDispInit()				�f�W�I�y�������					�\���f�[�^�쐬			*/
/*		PrmCopyStrCopy()				������R�s�[												*/
/*		PrmCopyStrCopy2()				������R�s�[(�R�s�[��̃A�h���X�X�V��̒l��Ԃ�)			*/
/*		-------------------------------------------------------------------------------------------	*/
/*		RpxLcdPrmCopyInit()				�p�����[�^�R�s�[�@�\������									*/
/*		RpxLcdPrmInfoGet()				�f�W�I�y�p�����[�^�u���b�N�Ǘ����擾						*/
/*		PrmCopyCmdRspCheck()			�f�W�I�y�ʐM�R�}���h�����m�F								*/
/*		PrmCopyPrmMngTblOp()			�f�W�I�y�p�����[�^�Ǘ���񑀍�								*/
/*		PrmCopyCmdInit()				�f�W�I�y�ʐM  ������										*/
/*		PrmCopyNormalCmdSet()			�f�W�I�y�ʐM  �ʏ�R�}���h(0x60)��Ɨ̈�Z�b�g				*/
/*		PrmCopyLoadCmdSet()				�f�W�I�y�ʐM  LOAD�R�}���h(0x61)��Ɨ̈�Z�b�g				*/
/*		PrmCopySaveCmdSet()				�f�W�I�y�ʐM  SAVE�R�}���h(0x62)��Ɨ̈�Z�b�g				*/
/*		PrmCopySetSendBuf()				���M�o�b�t�@�Z�b�g����										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�k�b�c�p�����[�^�R�s�[�h�e�֐�																*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdPrmCopyInit( void );			/* �p�����[�^�R�s�[�@�\����������						*/
LONG	RpxLcdPrmInfoGet( void );			/* �p�����[�^�u���b�N���擾����						*/
//LONG	RpxLcdPrmCopyMain( void );			/* �p�����[�^�R�s�[�@�\���C������						*/
void	RpxLcdPrmCopyMain( MSGIF *Hmsg, DRVSEL *pDrvSel );	/* �p�����[�^�R�s�[�@�\���C������		*/
/*--------------------------------------------------------------------------------------------------*/
/*		�k�b�c�p�����[�^�R�s�[���[�J���֐�															*/
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopyInit(void);
LONG	PrmCopyOpMain(void);
LONG	PrmCopyModeSelect(void);
void	PrmCopyModeExe(void);
void	PrmCopyBlockSelect(void);
void	PrmCopyExeConfirm(void);
void	PrmCopyLoadOrVerifyExe(void);
void	PrmCopySaveOrDeleteExe(void);
SHORT	PrmCopyLoadExePrmWrite(void);
SHORT	PrmCopyVerifyExePrmVerify(void);
void	PrmCopySaveKeyOp(void);
void	PrmCopyLoadKeyOp(void);
void	PrmCopyVerifyKeyOp(void);
void	PrmCopyErrorOp(void);
SHORT	PrmCopyModeSelectCheck(void);
void	PrmCopyDispMain(void);
void	PrmCopyDispError(void);
void	PrmCopyDispModeSelect(void);
void	PrmCopyDispBlockSelectMain(void);
void	PrmCopyDispBlockSelect(void);
void	PrmCopyDispExeConfirm(void);
void	PrmCopyDispSaveExe(void);
void	PrmCopyDispLoadExe(void);
void	PrmCopyDispVerifyExe(void);
void	PrmCopyDispVerifyUnmatchList(void);
void	PrmCopyDispDeleteExe(void);
void	PrmCopyDispBlockHeader(void);
void	PrmCopyMakeBlockDispLine( UCHAR *, LONG );
void	PrmCopyDispInit(void);
/*--------------------------------------------------------------------------------------------------*/
SHORT	PrmCopyBlockSelectCheck(void);
LONG	PrmCopyBlockTypeCheck( LONG BlockNo );
void	PrmCopyCmdInit(void);
SHORT	PrmCopyCmdRspCheck( USHORT );
void	PrmCopyPrmMngTblOp( UCHAR, USHORT );
void	PrmCopyStrCopy( UCHAR *, UCHAR * );
void	PrmCopyStrCopy2( UCHAR **, UCHAR * );
//SHORT	PrmCopyUsrPrmSearch( USHORT *, const PRMTBL *, const USHORT );
void	ConvertHexToStr( USHORT, UCHAR *, USHORT );
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopyNormalCmdSet( void );
void	PrmCopyLoadCmdSet( USHORT, USHORT );
void	PrmCopySaveCmdSet( USHORT, USHORT, USHORT * );
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopyLoadMngInfoCmdSet( LONG, LONG );
void	PrmCopySaveMngInfoCmdSet( LONG, LONG );
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopySetSendBuf(void);
USHORT	PrmCopyGetEntNum( const PRMTBL *, const USHORT );
//LONG	PrmCopyPrmRead( USHORT, const PRMTBL *, const USHORT, USHORT * );
/*--------------------------------------------------------------------------------------------------*/





/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�R�s�[�@�\����������																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdPrmCopyInit( void )
{
LONG idx, ret;
ULONG DriveID = 0x00;					/* <S073> */
ULONG IfMdleId = 0x00;					/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�R�s�[��Ɨp�ϐ��S�N���A															*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)&PrmCopyV, sizeof(PRM_COPY_INFO) );
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�R�s�[�Ǘ��p�p�����[�^�ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
#if		CCSW_HWID == HWID_JPCV
		PrmCopyP.TypeID = KsysP.DriveID;
#else
		PrmCopyP.TypeID = (((PtiReg.IfMdleId & 0x00FF) << 8) | (KsysP.DriveID & 0x00FF));
#endif
#endif
/*--------------------------------------------------------------------------------------------------*/
		ret = OBJ_SEARCH( CINFREG, 0xC000 & 0x0FFF /* <S073> */, &idx ); 	/* @@ 0xC000: DriveId */
		if ( ret == OK ) { OBJ_READ( CINFREG, idx, &DriveID ); }  			/* @@ 0xC000: DriveId */
/*--------------------------------------------------------------------------------------------------*/
/*	
 *		TODO:
 *		���W�X�^0xC013��Mercury�ɑ��݂��Ă��Ȃ����߁A�������ʂ�NG�ɂȂ�B
 *
 */
/*--------------------------------------------------------------------------------------------------*/
		ret = OBJ_SEARCH( CINFREG, 0xC013 & 0x0FFF /* <S073> */, &idx ); 	/* @@ 0xC013: IfModuleId */
		if ( ret == OK ){ OBJ_READ( CINFREG, idx, &IfMdleId );  }			/* @@ 0xC013: IfModuleId */
		PrmCopyP.TypeID = (((IfMdleId & 0x00FF) << 8) | (DriveID & 0x00FF));
/*--------------------------------------------------------------------------------------------------*/
/*		Free Trace Buffer																			*/
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->FreeBuffer( CTRCMEM, &PrmCopyV );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�R�s�[�@�\���C������																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdPrmCopyMain( MSGIF *Hmsg, DRVSEL *pDrvSel )
{
		PrmCopyInit();						/* ��������		 										*/
		/* ModeChg = */PrmCopyOpMain();		/* �L�[���상�C������									*/
		PrmCopyDispMain();					/* ��ʕ\���f�[�^�쐬���C������							*/
		PrmCopySetSendBuf();				/* �ʐM��Ɨ̈悩��A���M�o�b�t�@�փf�[�^�Z�b�g			*/
		//return( ModeChg );
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdSetResStatus ( (UCHAR *)Hmsg->ResBuf );	/* Set Status */
		RpxLcdSetBlink ( (UCHAR *)Hmsg->ResBuf );		/* Set Blink */
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Checks Parameter Copy Execution Status														*/
/*																									*/
/****************************************************************************************************/
LONG RpxChkPrmCopyExe( void )
{
	return ( PrmCopyV.MainMode == PRMCOPY_MAINMODE_MODE_EXE ); 
}


/****************************************************************************************************/
/*																									*/
/*		��������																					*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyInit( void )
{
		PrmCopyCmdInit();					/* �f�W�I�y�ʐM��������									*/
		PrmCopyNormalCmdSet(); 				/* �f�t�H���g�Œʏ�ʐM�R�}���h(0x60)�Z�b�g 			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�L�[���상�C������																			*/
/*																									*/
/****************************************************************************************************/
LONG	PrmCopyOpMain( void )
{
LONG	ModeChg = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		Key Operation Main																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.MainMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�@�\�I��																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_SELECT:
			ModeChg = PrmCopyModeSelect();
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�@�\���s																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_EXE:
			PrmCopyModeExe();
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			/* �G���[���������� */
			PrmCopyErrorOp();
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}



/****************************************************************************************************/
/*																									*/
/*		�@�\�I������																				*/
/*																									*/
/*			"1:SERVO->OP",		[00] : SAVE 														*/
/*			"2:OP->SERVO",		[01] : LOAD															*/
/*			"3:VERIFY",			[02] : VERIFY														*/
/*			"4:DELETE",			[03] : DELETE														*/
/*																									*/
/****************************************************************************************************/
LONG	PrmCopyModeSelect( void )
{
LONG	ModeChg = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		[MODE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			RpxLcdPrmCopyInit();					/* ����������(�\���̕ϐ��S�N���A���A����̂�)	*/
			PrmCopyInit();							/* ��������(�ʐM�o�b�t�@�N���A���A����)			*/
			ModeChg  = TRUE;						/* ���[�h�ύX�F�p�����[�^�ҏW�E���j�^��ʂ�		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[��] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.down )
		{
			if( PrmCopyV.SubMode < PRMCOPY_SUBMODE_MAX - 1 )
			{
				PrmCopyV.SubMode++;								/* ���̋@�\�� */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[��] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.up )
		{
			if( PrmCopyV.SubMode > 0 )
			{
				PrmCopyV.SubMode--;								/* �O�̋@�\�� */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[DATA] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.data )
		{
			if( PrmCopyModeSelectCheck() == RC_OK )				/* �@�\�I���`�F�b�N					*/
			{
				PrmCopyV.MainMode = PRMCOPY_MAINMODE_MODE_EXE;	/* �@�\���s����ԃZ�b�g				*/
				PrmCopyV.BlockNo = 0;							/* �I�𒆋@�\�m�n��������(�擪)		*/
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;	/* �I���@�\����������				*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}



/****************************************************************************************************/
/*																									*/
/*		�@�\���s����																				*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyModeExe( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		���s�����V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubModeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�@�\��������																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_INIT:
			/* �u���b�N�I���� */
			PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT;
		/*------------------------------------------------------------------------------------------*/
		/*		�u���b�N�I��																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT:
			PrmCopyBlockSelect();				/* �u���b�N�I������				 					*/
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		���s�J�n�m�F																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM:
			PrmCopyExeConfirm();				/* ���s�J�n�m�F����				 					*/
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		���s																				*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_EXE:
			switch( PrmCopyV.SubMode )
			{
				/*----------------------------------------------------------------------------------*/
				/*		SAVE(Servo -> DgOp)															*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SAVE:
					PrmCopySaveKeyOp();			/* Save�� �L�[���� 									*/
					PrmCopySaveOrDeleteExe();	/* Save�������s										*/
					break;
				/*----------------------------------------------------------------------------------*/
				/*		DELETE(DgOp)																*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_DELETE:
					PrmCopySaveOrDeleteExe();	/* Delete�������s									*/
					break;
				/*----------------------------------------------------------------------------------*/
				/*		LOAD(DgOp -> Servo)															*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_LOAD:
					PrmCopyLoadKeyOp();			/* Load�� �L�[���� 									*/
					PrmCopyLoadOrVerifyExe();	/* Load�������s										*/
					break;
				/*----------------------------------------------------------------------------------*/
				/*		VERIFY(Servo <-> DgOp)														*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_VERIFY:
					PrmCopyVerifyKeyOp();		/* VERIFY�� �L�[���� 								*/
					PrmCopyLoadOrVerifyExe();	/* VERIFY�������s									*/
					break;
				/*----------------------------------------------------------------------------------*/
				default:
					break;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�u���b�N�I������																	*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyBlockSelect(void)
{
SHORT	rc;
ULONG	wkx;
ULONG	BlockSft = (Lpx_ChkDoubleBlock( PrmCopyP.TypeID ) ? 1 : 0 ); 	/* @@ SGD7W�Ή� */
ULONG	BlockMax = ((PRMCOPY_MNG_INFO_BLOCK_MAX >> BlockSft) - 1);		/* @@ SGD7W�Ή� */
/*--------------------------------------------------------------------------------------------------*/
/*		[MODE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			PrmCopyV.MainMode = PRMCOPY_MAINMODE_MODE_SELECT;	/* �@�\�I����ʂ� */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[��] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.up )
		{
			/* �p�����[�^�u���b�N�\���X�N���[���A�b�v */
			wkx = PrmCopyV.BlockNo >> BlockSft;
			if( wkx == 0 )
			{
				wkx = BlockMax;
			}
			else
			{
				wkx--;
			}
			PrmCopyV.BlockNo = wkx << BlockSft;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[��] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.down )
		{
			/* �p�����[�^�u���b�N�\���X�N���[���_�E�� */
			wkx = PrmCopyV.BlockNo >> BlockSft;
			if( wkx == BlockMax )
			{
				wkx = 0;
			}
			else
			{
				wkx++;
			}
			PrmCopyV.BlockNo = wkx << BlockSft;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[DATA] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.data )
		{
			PrmCopyV.DispHoldTime = 0;					/* �Q�b�ԉ�ʕ\���J�E���^�[�N���A			*/
			rc = PrmCopyBlockSelectCheck();				/* �u���b�N�I���\�`�F�b�N					*/
			if( rc != RC_OK ){ return;}
			PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM;	/* ���s�J�n�m�F��ʂ�			*/
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_INIT;				/* ���s�V�[�P���X������������	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�u���b�N�I���`�F�b�N����															*/
/*																									*/
/****************************************************************************************************/
SHORT PrmCopyBlockSelectCheck(void)
{
/*--------------------------------------------------------------------------------------------------*/
/*		���s�����V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		SAVE(Servo -> DgOp)																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SAVE:
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD(DgOp -> Servo)																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_LOAD:
			/* "****"�\����I���������́A�f�[�^�Ȃ��G���[ */
			if( PrmMngTbl[PrmCopyV.BlockNo].FormatID == PRMCOPY_MNG_DATA_NONE )
			{
				/* �I��s�G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NO_DATA;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			/* �p�����[�^�^�����Ⴄ */
			if( PrmCopyBlockTypeCheck( PrmCopyV.BlockNo ) != RC_OK )
			{
				/* �p�����[�^�^���G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_PRM_UNMATCH;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY(Servo <-> DgOp)																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_VERIFY:
			/* "****"�\����I���������́A�f�[�^�Ȃ��G���[ */
			if( PrmMngTbl[PrmCopyV.BlockNo].FormatID == PRMCOPY_MNG_DATA_NONE )
			{
				/* �I��s�G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NO_DATA;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			/* �T�[�{�p�b�N�^�����Ⴄ */
			if( PrmMngTbl[PrmCopyV.BlockNo].ServoID != PrmCopyP.TypeID )
			{
				/* �p�����[�^�^���G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_PRM_UNMATCH;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		DELETE(DgOp)																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_DELETE:
			/* "****"�\����I���������́A�f�[�^�Ȃ��G���[ */
			if( PrmMngTbl[PrmCopyV.BlockNo].FormatID == PRMCOPY_MNG_DATA_NONE )
			{
				/* �I��s�G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NO_DATA;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}


/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�u���b�N�^�C�v�`�F�b�N����														*/
/*																									*/
/****************************************************************************************************/
LONG	PrmCopyBlockTypeCheck( LONG BlockNo )
{
PRMDATA *pPrm = LopMsg.pAxRsc->UniPrms->Prm;			/* TODO: ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		Block Type Check																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( PrmMngTbl[BlockNo].ServoID != PrmCopyP.TypeID )
		{
			return( RC_NG );
		}
//		else if( PrmMngTbl[BlockNo].FormatID > FORMAT_ID )
		else if( PrmMngTbl[BlockNo].FormatID > EEP_FORMAT_ID )
		{
			return( RC_NG );
		}
		else if( PrmMngTbl[BlockNo].AmpWatt != pPrm->ampw )
		{
			return( RC_NG );
		}
		else if( (PrmMngTbl[BlockNo].AmpVolt & 0x0F) != (pPrm->styp.b.h & 0x0F) )
		{
			return( RC_NG );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}



/****************************************************************************************************/
/*																									*/
/*		���s�J�n�m�F����																			*/
/*																									*/
/****************************************************************************************************/
void PrmCopyExeConfirm(void)
{
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;			/* TODO: ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		[MODE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			/* �@�\����������A�u���b�N�I���֖߂� */
			PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[READ] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.read )
		{
			/* SAVE(Servo -> DgOp) or VERIFY(Servo <-> DgOp)������ */
			if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_SAVE || PrmCopyV.SubMode == PRMCOPY_SUBMODE_VERIFY )
			{
				PrmCopyV.TraceBuffer = CTRCMEM->GetBuffer( CTRCMEM, &PrmCopyV );	/* get buffer 	*/
				if ( PrmCopyV.TraceBuffer != NULL )
				{
					/* �@�\�������s�� */
					PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_EXE;
				}
				else
				{
					/* ���s�s�G���[�\����ʂ� */
					PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
					PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[WRITE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.write )
		{
			/* LOAD(DgOp -> Servo) or DELETE(DgOp)������	*/
			if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD || PrmCopyV.SubMode == PRMCOPY_SUBMODE_DELETE )
			{
				/* �T�[�{�I�����͎��s�s�G���[	*/
				if( ! CHK_BB() /*RinV.f.BaseEnable*/ )
				{
					/* ���s�s�G���[�\����ʂ� */
					PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
					PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				}
				else if ( ( PrmCopyV.TraceBuffer = CTRCMEM->GetBuffer( CTRCMEM, &PrmCopyV ) ) == NULL )
				{
					/* ���s�s�G���[�\����ʂ� */
					PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
					PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				}
				else
				{
					/* �@�\�������s�� */
					PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_EXE;
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		SAVE(Servo -> DgOp) or DELETE(DgOp)���s����													*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveOrDeleteExe( void )
{
SHORT	rc;
USHORT	prm_data;
USHORT	cmd_data_idx;
USHORT	send_data[LCD_MSG_DATA_GROUP_NUM];
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;		/* TODO: ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*==========================================================================================*/
		/*	�O�D��������																			*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		���s��������																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_INIT:
			PrmCopyV.RetryCnt = 0;								/* �ʐM���g���C�J�E���g�N���A		*/
			PrmCopyV.DispHoldTime = 0;							/* �Q�b�ԉ�ʕ\���J�E���^�[�N���A	*/
			PrmCopyV.DispKeepTime = 0;
			PrmCopyV.BlockMngAreaNo = 0;						/* �u���b�N�Ǘ����̈�O�̍폜		*/
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ;	/* �w�b�_���폜�R�}���h�v����		*/
			break;
		/*==========================================================================================*/
		/*	�P�D�f�W�I�yEEPROM�̊Y���p�����[�^�u���b�N�̊Ǘ������폜								*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		�w�b�_���폜�R�}���h�v��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ:
			/* �f�W�I�y �p�����[�^�u���b�N�Ǘ����N���A */
			PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_CLEAR, PrmCopyV.BlockNo );
			/* �ʐM��Ɨ̈��SAVE�R�}���h(0x62)�Z�b�g���� */
			PrmCopySaveMngInfoCmdSet( PrmCopyV.BlockNo, PrmCopyV.BlockMngAreaNo );
			/* �w�b�_���폜�R�}���h�����m�F�� */
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�w�b�_���폜�R�}���h�����m�F														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP:
			/* �f�W�I�y�ʐM�R�}���h�����m�F���� */
			rc = PrmCopyCmdRspCheck( LCD_CMD_SAVE );
			/*----- ��M���� -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* �ʐM���g���C�J�E���g�N���A */
				PrmCopyV.RetryCnt = 0;
//				PrmCopyV.PrmIndex = 0;
				PrmCopyV.PrmReadIndex = 0;
				PrmCopyV.PrmWriteIndex = 0;
			  /*------------------------------------------------------------------------------------*/
				if( PrmCopyV.BlockMngAreaNo == 0 )						/* �Ǘ����̈�O�̏ꍇ		*/
				{
					PrmCopyV.BlockMngAreaNo = 1;						/* �Ǘ����̈�P�̍폜		*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ;	/* �w�b�_���폜����		*/
				}
				else if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_SAVE )		/* �p�����[�^�ۑ������̏ꍇ	*/
				{
//Mercury			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;		/* �p�����[�^�f�[�^���M��	*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_READ_REQ;	/* @@ �p�����[�^�f�[�^�Ǐo��*/
				}
				else													/* �p�����[�^�폜�����̏ꍇ	*/
				{
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_COMPLETE;		/* ������ʕ\����			*/
				}
			}
			/*----- �A���[���R�[�h����M���� -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* �w�b�_���폜�R�}���h�v���֖߂� */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ;
			}
			/*----- ���g���C�񐔃I�[�o�[ -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* �������f�� */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			/*----- ��M�����R�}���h�R�[�h�����M�����R�[�h�ƈႤ -----------------------------------*/
			else
			{
				;/* ����������M�҂�(rc == RC_PASS) */
			}
			break;
		/*==========================================================================================*/
		/*	�Q�D�f�W�I�yEEPROM�ցA�S�Ẵ��[�U�[�p�����[�^��8WORD�Â��M							*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		Read Upload Data from Eeprom														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_REQ:
			if ( KriRequestReadLcdPdlEeprom( pAxRsc->pRwPdlMng, PrmCopyV.TraceBuffer, 
									(void *)&LcdPdlTbl[0], LCDPDLTBL_ENTNUM  ) == OK )
			{
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_READ_WAIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Wait for Completing Read Data from Eeprom											*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_WAIT:
			if (  KriChkDoneReadLcdPdlEeprom( pAxRsc->pRwPdlMng ) )
			{
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^�f�[�^�`���R�}���h���M													*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_SEND:
			/* ���g���C���ɁA�C���f�b�N�X��߂��ׂ̕ۑ� */
			PrmCopyV.PrmReadIndex_Last = PrmCopyV.PrmReadIndex;
			PrmCopyV.PrmWriteIndex_Last = PrmCopyV.PrmWriteIndex;
//			PrmCopyV.PrmIndex_Last = PrmCopyV.PrmIndex;
			/* �P�񑗐M���̂W�̃p�����[�^�f�[�^���擾 */
			for( cmd_data_idx = 0; cmd_data_idx < LCD_MSG_DATA_GROUP_NUM; cmd_data_idx++ )
			{
				if ( PrmCopyV.PrmReadIndex >= LCDPDLTBL_ENTNUM ) { break; }	/* Data End */
				prm_data = PrmCopyV.TraceBuffer[ PrmCopyV.PrmReadIndex ];
				/* ���M�o�b�t�@�Ƀp�����[�^�f�[�^���Z�b�g */
				send_data[cmd_data_idx] = prm_data;
				PrmCopyV.PrmReadIndex++;
			}
			/* �f�W�I�yEEPROM�p�����[�^�A�h���X�X�V */
			PrmCopyV.PrmWriteIndex += cmd_data_idx;
			/* �ʐM��Ɨ̈��SAVE�R�}���h(0x62)�Z�b�g���� */
			PrmCopySaveCmdSet( PRMCOPY_PRM_ADDR_GET(PrmCopyV.BlockNo, PrmCopyV.PrmWriteIndex_Last), 
							cmd_data_idx, send_data );
			/* �p�����[�^�f�[�^�`���R�}���h�����m�F�� */
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^�f�[�^�`���R�}���h�����m�F												*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_RSP:
			/* �f�W�I�y�ʐM�R�}���h�����m�F���� */
			rc = PrmCopyCmdRspCheck( LCD_CMD_SAVE );
			/*----- ��M���� -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* �ʐM���g���C�J�E���g�N���A */
				PrmCopyV.RetryCnt = 0;
				/* �p�����[�^�f�[�^��S�đ��M���� */
				if( PrmCopyV.PrmReadIndex >= PrmEntNum.SvPrm )
				{
					PrmCopyV.BlockMngAreaNo = 0;					/* �Ǘ����̈�O�̏�������		*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_SEND;	/* ���̃R�}���h(�Ǘ����`��)�� */
				}
				else
				{
					/* �����������̃p�����[�^�f�[�^�`�����s�� */
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;
				}
			}
			/*----- �A���[���R�[�h����M���� -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* �p�����[�^�C���f�b�N�X�����ɖ߂� */
				PrmCopyV.PrmReadIndex = PrmCopyV.PrmReadIndex_Last;
				PrmCopyV.PrmWriteIndex = PrmCopyV.PrmWriteIndex_Last;
//				PrmCopyV.PrmIndex = PrmCopyV.PrmIndex_Last;
				/* �p�����[�^�f�[�^�`���R�}���h���M�֖߂��ă��g���C */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;
			}
			/*----- ���g���C�񐔃I�[�o�[ -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* �������f�� */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			/*----- ��M�����R�}���h�R�[�h�����M�����R�[�h�ƈႤ -----------------------------------*/
			else /* if( rc == RC_PASS ) */
			{
				;/* ����������M�҂� */
			}
			break;
		/*==========================================================================================*/
		/*	�R�D�f�W�I�yEEPROM�ցA�Y���p�����[�^�u���b�N�̊Ǘ����𑗐M							*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		�w�b�_���o�^�R�}���h���M															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_SEND:
			/* �f�W�I�y �p�����[�^�u���b�N�Ǘ����Z�b�g */
			PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_SET, PrmCopyV.BlockNo );
			/* �ʐM��Ɨ̈��SAVE�R�}���h(0x62)�Z�b�g���� */
			PrmCopySaveMngInfoCmdSet( PrmCopyV.BlockNo, PrmCopyV.BlockMngAreaNo );
			/* �w�b�_���o�^�R�}���h�����m�F�� */
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�w�b�_���o�^�R�}���h�����m�F														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_RSP:
			/* �f�W�I�y�ʐM�R�}���h�����m�F */
			rc = PrmCopyCmdRspCheck( LCD_CMD_SAVE );
			/*----- ��M���� -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				PrmCopyV.RetryCnt = 0;								/* �ʐM���g���C�J�E���g�N���A	*/
				if( PrmCopyV.BlockMngAreaNo == 0 )					/* �Ǘ����̈�O�̏ꍇ			*/
				{
					PrmCopyV.BlockMngAreaNo = 1;					/* �Ǘ����̈�P�̏�������		*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_SEND;	/* �Ǘ���񑗐M��				*/
				}
				else
				{
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_COMPLETE;	/* ������ʕ\����				*/
				}
			}
			/*----- �A���[���R�[�h����M���� -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* �w�b�_���o�^�R�}���h���M�֖߂� */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_SEND;
			}
			/*----- ���g���C�񐔃I�[�o�[ -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* �������f�� */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			/*----- ��M�����R�}���h�R�[�h�����M�����R�[�h�ƈႤ -----------------------------------*/
			else
			{
				;/* ����������M�҂� */
			}
			break;
		/*==========================================================================================*/
		/*	�S�D�f�W�I�y���s������ʁA���́A���f��ʂ��Q�b�ԕ\�����邽�߂Ɏ��s�V�[�P���X�ێ�		*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���s������ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_COMPLETE:
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���f��ʕ\����																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_CANCEL:
			/* ��ʕ\������ */
			if( PrmCopyV.DispHoldTime == 0 )
			{
				/* �^�C�}�[���Z�b�g */
				KpiRstLongTimer( &PrmCopyV.DispHoldTime );
				break;
			}
			/* �Q�b�Ԍo�߂��� */
			if( KpiGetLongTimer(&PrmCopyV.DispHoldTime) > TIME_SECOND_2 )
			{
				CTRCMEM->FreeBuffer( CTRCMEM, &PrmCopyV );	/* @@ */
				/* �@�\����������A�u���b�N�I���֖߂� */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LOAD(DgOp -> Servo)�A���́AVERIFY(Servo <-> DgOp)���s����									*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadOrVerifyExe( void )
{
SHORT	rc, rc2;
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;		/* TODO: ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*==========================================================================================*/
		/*	�O�D��������																			*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		���s��������																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_INIT:
			/* �p�����[�^�R�s�[�@�\�ɂ����āA�p�����[�^�ꊇ�����������t���O��ON */
			pAxRsc->FnCmnCtrl->f.PrmCopy = TRUE; /* RoutV.f.PrmCopy = TRUE; */
			/* �ʐM���g���C�J�E���g�N���A */
			PrmCopyV.RetryCnt = 0;
			PrmCopyV.PrmReadIndex = 0;
			PrmCopyV.PrmWriteIndex = 0;
//			PrmCopyV.PrmIndex = 0;
//			PrmCopyV.UserChkSum = 0;
			PrmCopyV.VerifyErrTblCnt = 0;
			PrmCopyV.VerifyErrTotalCnt = 0;
			/* �Q�b�ԉ�ʕ\���J�E���^�[�N���A */
			PrmCopyV.DispHoldTime = 0;
			PrmCopyV.DispKeepTime = 0;
			/* VERIFY������ */
			if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_VERIFY )
			{
				/* �p�����[�^READ�R�}���h�v���� */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_FILL_REQ;
				break;
			}
			/* LOAD�������A���s�O�Ċm�F�� */
			PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	@@	Read Verify Data from Eeprom														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_FILL_REQ:
			if ( KriRequestReadLcdPdlEeprom( pAxRsc->pRwPdlMng, PrmCopyV.TraceBuffer, 
									(void*)&LcdPdlTbl[0], LCDPDLTBL_ENTNUM  ) == OK )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_FILL_WAIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Wait for Completing Read Data from Eeprom											*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_FILL_WAIT:
			if ( KriChkDoneReadLcdPdlEeprom( pAxRsc->pRwPdlMng ) )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD�������A���s�O�Ċm�F����														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_RECONFIRM:
			break;
		/*==========================================================================================*/
		/*	�P�D�f�W�I�yEEPROM����Y���u���b�N�̃��[�U�[�p�����[�^��								*/
		/*		8WORD�Â�M���Ȃ���A�T�[�{�p�b�N��EEPROM�֏������ނ��A���́A						*/
		/*		�T�[�{�p�b�N�̃p�����[�^��VERIFY���s��												*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^READ�R�}���h�v��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_REQ:
			/* �ʐM��Ɨ̈��LOAD�R�}���h(0x61)�Z�b�g */
			PrmCopyLoadCmdSet( PRMCOPY_PRM_ADDR_GET(PrmCopyV.BlockNo, PrmCopyV.PrmReadIndex), 
							LCD_MSG_DATA_GROUP_NUM );
			/* �w�b�_���폜�R�}���h�����m�F�� */
			PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�R�}���h�����m�F																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_RSP:
			/* �f�W�I�y�ʐM�R�}���h�����m�F���� */
			rc = PrmCopyCmdRspCheck( LCD_CMD_LOAD );
			/*----- ��M���� -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* �ǂݍ��ݐ�C���f�b�N�X���X�V(+8) */
				PrmCopyV.PrmReadIndex += LCD_MSG_DATA_GROUP_NUM;
				/* �����^�C�v���� */
				if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD )
				{
					/********************************************************************************/
					/*	�P���M���̍ő�W�̃p�����[�^���T�[�{�p�b�N��EEPROM�֏������ޏ��� 		*/
					/********************************************************************************/
					rc2 = PrmCopyLoadExePrmWrite();
				}
				else	/* PRMCOPY_SUBMODE_VERIFY */
				{
					/********************************************************************************/
					/* 	�P���M���̍ő�W�̃p�����[�^�� 											*/
					/* 		�T�[�{�p�b�N�̃p�����[�^(EEPROM)��VERIFY���s��							*/
					/********************************************************************************/
					rc2 = PrmCopyVerifyExePrmVerify();
				}
				/* �p�����[�^�������݊��� */
				if( rc2 == RC_OK )
				{
					/* �ʐM���g���C�J�E���g�N���A */
					PrmCopyV.RetryCnt = 0;
					/* �����������̃f�[�^��M���s�� */
					PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
					break;
				}
				/* �p�����[�^�S�ď������݊��� -> �p�����[�^���ׂĎ�M����<S00?> */
				else if( rc2 == RC_COMPLETE )
				{
					if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD )
					{
						/* �S���[�U�p�����[�^��EEPROM�ɏ����� */
						PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_STORE_REQ;
					}
					else
					{
						/* ���������� */
						PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_COMPLETE;
					}
				}
				/* �p�����[�^�������ݗv�����s */
				else if( rc2 == RC_NG )
				{
					/* �ēx�A�����p�����[�^�f�[�^��M�� */
					PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
					PrmCopyV.PrmReadIndex = PrmCopyV.PrmWriteIndex;
				}
			}
			/*----- �A���[���R�[�h����M���� -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* �ēx�A�����p�����[�^�f�[�^��M�� */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			}
			/*----- ���g���C�񐔃I�[�o�[ -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* �������f�� */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_CANCEL;
			}
			/*----- ��M�����R�}���h�R�[�h�����M�����R�[�h�ƈႤ -----------------------------------*/
			else
			{
				/* ����������M�҂� */
			}
			break;
		/*==========================================================================================*/
		/*	�Q�D�f�W�I�y���s������ʁA���́A���f��ʂ��Q�b�ԕ\�����邽�߂Ɏ��s�V�[�P���X�ێ�		*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���s������ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_COMPLETE:
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���f��ʕ\����																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_CANCEL:
			/* ��ʕ\������ */
			if( PrmCopyV.DispHoldTime == 0 )
			{
				/* �p�����[�^�R�s�[�@�\�ɂ����āA�p�����[�^�ꊇ�����������t���O��OFF */
				//RoutV.f.PrmCopy = FALSE;
				pAxRsc->FnCmnCtrl->f.PrmCopy = FALSE;			/* @@ chk */
				/* �Q�b�ԕ\���^�C�}�[���Z�b�g */
				KpiRstLongTimer( &PrmCopyV.DispHoldTime );
				break;
			}
			/* �Q�b�ԕ\������ */
			if( KpiGetLongTimer(&PrmCopyV.DispHoldTime) > TIME_SECOND_2 )
			{
				CTRCMEM->FreeBuffer( CTRCMEM, &PrmCopyV );	/* @@ */
				PrmCopyV.DispHoldTime = 0;
				/* LOAD ������ */
				if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD )
				{
//<S0AA>			KPI_WARNINGSET(WRN_BOOT);		/* A.941 : �d���ē����v���[�U�[�萔�ύX			*/
					ALMSetServoAlarm( pAxRsc->AlmManager, WRN_BOOT );	/* <S0AA> A.941 : */
					/* �@�\����������A�u���b�N�I���֖߂� */
					PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
				}
				else	/* VERIFY ������ */
				{
					/* �L�����Z���̏ꍇ */
					if( PrmCopyV.ExeSeq == PRMCOPY_LOAD_SEQ_CANCEL )
					{
						/* �@�\����������A�u���b�N�I���֖߂� */
						PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
					}
					else if( PrmCopyV.VerifyErrTotalCnt == 0 )	/* VERIFY���� �S��v */
					{
						/* VERIFY���� ��v��ʕ\���� */
						PrmCopyV.ExeSeq = PRMCOPY_VERIFY_SEQ_MATCHED;
					}
					else	/* VERIFY���� �s��v���� */
					{
						PrmCopyV.VerifyErrListNo = 0;
						/* VERIFY���ʕs��v��ʕ\���� */
						PrmCopyV.ExeSeq = PRMCOPY_VERIFY_SEQ_UNMATCHED;
					}
				}
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY���ʈ�v��ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_MATCHED:
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY���ʕs��v��ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_UNMATCHED:
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Store Parameters to Eeprom															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_STORE_REQ:
		/*------------------------------------------------------------------------------------------*/
			if ( KriRequestWriteLcdPdlEeprom( pAxRsc->pRwPdlMng, PrmCopyV.TraceBuffer, 
									(void*)&LcdPdlTbl[0], LCDPDLTBL_ENTNUM  ) == OK )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_STORE_WAIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Store Parameters to Eeprom															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_STORE_WAIT:
			if (  KriChkDoneWriteLcdPdlEeprom( pAxRsc->pRwPdlMng ) )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_COMPLETE;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		SAVE(Servo -> DgOp)��  Key����																*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveKeyOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		case PRMCOPY_SAVE_SEQ_COMPLETE :	/* �f�W�I�y���s������ʕ\���� */
			break;
		default :
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* ���s�������f�� */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LOAD(DgOp -> Servo)��  Key����																*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadKeyOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD�������A���s�O�Ċm�F��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_RECONFIRM:
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* �@�\����������A�u���b�N�I���֖߂� */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			/*-----	[DATA] key ---------------------------------------------------------------------*/
			else if( OpKeyMk.data )
			{
				/* �p�����[�^READ�R�}���h�v���� */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		VERIFY(�T�[�{�p�b�N <-> �f�W�I�y)��  Key����												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyVerifyKeyOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY���� ��v��ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_MATCHED:
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* �@�\����������A�u���b�N�I���֖߂� */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY���� �s��v��ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_UNMATCHED:
			/*----- [MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* �@�\����������A�u���b�N�I���֖߂� */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			/*----- [��] key -----------------------------------------------------------------------*/
			else if( OpKeyMk.up )
			{
				/* �p�����[�^�u���b�N�\���X�N���[���A�b�v */
				if( PrmCopyV.VerifyErrListNo > 0 )
					PrmCopyV.VerifyErrListNo--;
			}
			/*----- [��] key -----------------------------------------------------------------------*/
			else if( OpKeyMk.down )
			{
				/* �p�����[�^�u���b�N�\���X�N���[���_�E�� */
				if( PrmCopyV.VerifyErrListNo + PRMCOPY_DISP_LIST_MAX < PrmCopyV.VerifyErrTblCnt )
					PrmCopyV.VerifyErrListNo++;
			}
			break;
		default:
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* �������f���b�Z�[�W��ʕ\���� */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_CANCEL;
			}
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�P���M���̍ő�W�̃p�����[�^���T�[�{�p�b�N��EEPROM�֏������ޏ���						*/
/*		<MERCURY>EEPROM�����݃L���[�͎g��Ȃ������ɕύX												*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyLoadExePrmWrite(void)
{
USHORT	cmd_data_idx;
USHORT	dgop_prm_data;
/*--------------------------------------------------------------------------------------------------*/
/*		�P���M���̍ő�W�̃p�����[�^������														*/
/*--------------------------------------------------------------------------------------------------*/
		for( cmd_data_idx = 0; cmd_data_idx < LCD_MSG_DATA_GROUP_NUM; cmd_data_idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		��M�����p�����[�^���擾															*/
		/*------------------------------------------------------------------------------------------*/
			dgop_prm_data = PRMCOPY_CMD_DATA_GET( cmd_data_idx );
		/*------------------------------------------------------------------------------------------*/
			if (( PrmCopyV.PrmWriteIndex >= 2 ) && ( PrmCopyV.PrmWriteIndex >= PrmCopyV.TraceBuffer[ 1 ] ))
			{
				return RC_COMPLETE; 
			}
			PrmCopyV.TraceBuffer[ PrmCopyV.PrmWriteIndex ] = dgop_prm_data;
			PrmCopyV.PrmWriteIndex++;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}



/****************************************************************************************************/
/*																									*/
/*		�P���M���̍ő�W�̃p�����[�^�ƃT�[�{�p�b�N�̃p�����[�^(EEPROM)��VERIFY���s������		*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyVerifyExePrmVerify(void)
{
USHORT	cmd_data_idx;
USHORT	prm_data, dgop_prm_data;
//USHORT	nPrmVerify;	
/*--------------------------------------------------------------------------------------------------*/
/*		�P���M���̂W�̃p�����[�^�f�[�^������													*/
/*--------------------------------------------------------------------------------------------------*/
		for( cmd_data_idx = 0; cmd_data_idx < LCD_MSG_DATA_GROUP_NUM; cmd_data_idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^���擾																	*/
		/*------------------------------------------------------------------------------------------*/
			/* �T�[�{���p�����[�^�擾 */
			prm_data = PrmCopyV.TraceBuffer[ PrmCopyV.PrmWriteIndex ];
			/* �f�W�I�y�����M�����p�����[�^���擾 */
			dgop_prm_data = PRMCOPY_CMD_DATA_GET( cmd_data_idx );
		/*------------------------------------------------------------------------------------------*/
		/*		�f�[�^�����擾																		*/
		/*------------------------------------------------------------------------------------------*/
			if ( PrmCopyV.PrmWriteIndex == 1 )
			{
				PrmCopyV.VerifyNumber = MlibMIN( prm_data, dgop_prm_data );
			}
		/*------------------------------------------------------------------------------------------*/
		/*		��Prm�ƐVServo�̔�r�F��Prm�ɂȂ��p�����[�^�͔�r���Ȃ��B							*/
		/*		�VPrm�Ƌ�Servo�̔�r�F��Servo�ɂȂ��p�����[�^�͔�r���Ȃ��B							*/
		/*------------------------------------------------------------------------------------------*/
			else if ( PrmCopyV.PrmWriteIndex >= 2 ) 
			{
				//nPrmVerify = MlibMIN( PrmCopyV.TraceBuffer[1], LcdPdlTbl[1].PnNo );
				if ( PrmCopyV.PrmWriteIndex >= PrmCopyV.VerifyNumber /* nPrmVerify */)
				{
					return( RC_COMPLETE );				/* �S�����I��								*/
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*		�f�[�^�s��v																		*/
		/*------------------------------------------------------------------------------------------*/
			if (( LcdPdlTbl[ PrmCopyV.PrmWriteIndex ].pPrmDef != NULL ) && ( prm_data != dgop_prm_data ))
			{
				PrmCopyV.VerifyErrTotalCnt++;			/* VERIFY�G���[���������C���N�������g		*/
				if( PrmCopyV.VerifyErrTblCnt < PRMCOPY_VERIFY_ERR_DISP_TBL_MAX )
				{
					PrmCopyV.VerifyErrPrmIdxTbl[PrmCopyV.VerifyErrTblCnt] = PrmCopyV.PrmWriteIndex;
					PrmCopyV.VerifyErrTblCnt++;
				}
			}
		/*------------------------------------------------------------------------------------------*/
			PrmCopyV.PrmWriteIndex++;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}



/****************************************************************************************************/
/*																									*/
/*		�@�\�I���`�F�b�N����																		*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyModeSelectCheck(void)
{
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;
/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�����s�\���ǂ����`�F�b�N���s��														*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubMode )
		{
		/*------------------------------------------------------------------------------*/
		/*		SAVE(Servo -> DgOp)														*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SAVE:
			break;
		/*------------------------------------------------------------------------------*/
		/*		DELETE(DgOp)															*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_DELETE:
			break;
		/*------------------------------------------------------------------------------*/
		/*		LOAD(DgOp -> Servo)														*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_LOAD:
			/* �T�[�{�I�����͎��s�s�G���[	*/
			if( ! CHK_BB() /*RinV.f.BaseEnable*/ )
			{
				/* ���s�s�G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			/* �p�����[�^�������݋֎~���͎��s�s�G���[ */
			if( CHK_WRITEINHIBIT() /* Iprm.f.PrmWrtInhibit */ && (LopMsg.AccessLvl != ACCLVL_SYSTEM) )
			{
				/* ���s�s�G���[�\����ʂ� */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			pAxRsc->FnCmnCtrl->KnlMode = KPI_KMD_PARAMCPY;	/* @@ Executing Parameter Copy */
			break;
		/*------------------------------------------------------------------------------*/
		/*		VERIFY(Servo <-> DgOp)													*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_VERIFY:
			break;
		/*------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}



/****************************************************************************************************/
/*																									*/
/*		�G���[����������																			*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyErrorOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�G���[����������																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ErrorNo )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		���s�s�G���[																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NOT_AVAILABLE:
		/*------------------------------------------------------------------------------------------*/
		/*		�I��s�G���[																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NO_DATA:
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^�^���G���[																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_PRM_UNMATCH:
			/* ��ʕ\������ */
			if( PrmCopyV.DispHoldTime == 0 )
			{
				/* �^�C�}�[���Z�b�g */
				KpiRstLongTimer( &PrmCopyV.DispHoldTime );
			}
			/* �Q�b�ԕ\������ */
			if( KpiGetLongTimer(&PrmCopyV.DispHoldTime) > TIME_SECOND_2 )
			{
				/* �Q�b�ԉ�ʕ\���J�E���^�[�N���A */
				PrmCopyV.DispHoldTime = 0;
				/* �G���[��ԃN���A */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NONE;
				PrmCopyV.MainMode &= ~PRMCOPY_MAINMODE_ERROR_BIT;
				break;
			}
			return;
		/*------------------------------------------------------------------------------------------*/
		/*		�G���[�����Ȃ�																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NONE:
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		���[�U�[�p�����[�^��������																	*/
/*																									*/
/****************************************************************************************************/
#if 0
SHORT	PrmCopyUsrPrmSearch( USHORT *SrchIdx, const PRMTBL *PrmTbl, const USHORT EntNum )
{
CPRMDEF	*PrmDefPtr;
ULONG	EntNumX = 2 * EntNum;
/*--------------------------------------------------------------------------------------------------*/
/*		Check LongPrm																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( LPX_GET_HiWdFlag( (*SrchIdx) ) )
		{
			return( RC_OK );						/* �Y���p�����[�^���� (LongHighWord)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�S�p�����[�^�̒�����A���[�U�[�p�����[�^������												*/
/*--------------------------------------------------------------------------------------------------*/
		for(   ; *SrchIdx < EntNumX; LPX_INC_SrhIndex( (*SrchIdx) ) )
		{
			PrmDefPtr = LPX_GET_PrmDefPtr( (*SrchIdx) );
			if(	LPX_CHK_EEP_PRM( PrmDefPtr ) )
			{
				UCHAR  AccLvl = PrmDefPtr->AccessLevel;
				if( (AccLvl == ACCLVL_USER1) || (AccLvl == ACCLVL_USER2) )
				{
					return( RC_OK );				/* �Y���p�����[�^����							*/
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_NG );							/* �Y���p�����[�^�Ȃ�(�S�����I��)				*/
}
#endif



/****************************************************************************************************/
/*																									*/
/*		�g�d�w���當����ւ̕ϊ�����																*/
/*																									*/
/****************************************************************************************************/
void	ConvertHexToStr( USHORT HexData, UCHAR *str, USHORT digit )
{
LONG	i,x;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Digit																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (digit == 0) || (8 < digit) )
		{
			return;
		}
		if( digit < 8 )
		{
			HexData &= ((0x1L << (digit*4)) -1);		/* ��ʌ��𗎂Ƃ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Convert to Hex String																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < digit; i++, str++ )
		{
			x = (HexData >> ((digit-1-i)*4)) & 0x0F;
			if( x < 0xA ){ *str = (UCHAR)('0' + x);}
			else		 { *str = (UCHAR)('A' + (x - 0xA));}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		��ʕ\���f�[�^�쐬�������C��																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*										 01234567890123456											*/
/*		��ʕ\���G���A(17�� X 5�s)		0															*/
/*										1															*/
/*										2															*/
/*										3															*/
/*										4															*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispMain( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		��ʕ\���f�[�^�쐬����																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.MainMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�@�\�I����																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_SELECT:
			PrmCopyDispModeSelect();
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�@�\���s��																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_EXE:
			switch( PrmCopyV.SubModeSeq )
			{
				/*----------------------------------------------------------------------------------*/
				/*		�u���b�N�I��																*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT:
					PrmCopyDispBlockSelect();
					break;
				/*----------------------------------------------------------------------------------*/
				/*		���s�J�n�m�F																*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM:
					PrmCopyDispExeConfirm();
					break;
				/*----------------------------------------------------------------------------------*/
				/*		�������s��																	*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SEQ_EXE:
					switch( PrmCopyV.SubMode )
					{
						/*--------------------------------------------------------------------------*/
						/*		SAVE(Servo -> DgOp)���s��											*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_SAVE:
							PrmCopyDispSaveExe();
							break;
						/*--------------------------------------------------------------------------*/
						/*		LOAD(DgOp -> Servo)���s��											*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_LOAD:
							PrmCopyDispLoadExe();
							break;
						/*--------------------------------------------------------------------------*/
						/*		VERIFY(Servo <-> DgOp)���s��										*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_VERIFY:
							PrmCopyDispVerifyExe();
							break;
						/*--------------------------------------------------------------------------*/
						/*		DELETE(DgOp)���s��													*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_DELETE:
							PrmCopyDispDeleteExe();
							break;
						/*--------------------------------------------------------------------------*/
						default:
							break;
					}
					break;
				/*----------------------------------------------------------------------------------*/
				default:
					break;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			PrmCopyDispError();		/* �G���[���b�Z�[�W��ʁ@�\���f�[�^�쐬 */
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�G���[���b�Z�[�W��ʁ@�\���f�[�^�쐬����													*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispError(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�G���[���b�Z�[�W�\��																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ErrorNo )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		���s�s�G���[																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NOT_AVAILABLE:
			/* �P�s�ڂɃp�����[�^�R�s�[���C���^�C�g�� "-COPY-" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
							(UCHAR *)PrmCopyDispTitleTbl[PRMCOPY_DISPTITLE_MAIN] );
			/* �S�s�ڂ� "Not Available" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_NOT_AVAILABLE] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�I��s�G���[																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NO_DATA:
			/* �p�����[�^�u���b�N�I����A��ʃw�b�_�[�\�� */
			PrmCopyDispBlockHeader();
			/* �S�s�ڂ� "No Data" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_NO_DATA] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^�^���G���[																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_PRM_UNMATCH:
			/* �p�����[�^�u���b�N�I����A��ʃw�b�_�[�\�� */
			PrmCopyDispBlockHeader();
			/* �R�s�ڂ� "Unmatched" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_UNMATCHED_RIGHT] );
			/* �S�s�ڂ� "Parameters" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_RIGHT] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�G���[�����Ȃ�																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NONE:
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�@�\�I����ʁ@�\���f�[�^�쐬����															*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispModeSelect(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�P�s�ڂɃp�����[�^�R�s�[���C���^�C�g�� "-COPY-" ��\��										*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
							(UCHAR *)PrmCopyDispTitleTbl[PRMCOPY_DISPTITLE_MAIN] );
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�s�ڂɋ@�\�P "1:SERVO -> OP" ��\��														*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_2],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_SAVE] );
/*--------------------------------------------------------------------------------------------------*/
/*		�R�s�ڂɋ@�\�Q "2:OP -> SERVO" ��\��														*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_LOAD] );
/*--------------------------------------------------------------------------------------------------*/
/*		�S�s�ڂɋ@�\�R "3:VERIFY" ��\��															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_VERIFY] );
/*--------------------------------------------------------------------------------------------------*/
/*		�T�s�ڂɋ@�\�S "4:DELETE" ��\��															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_DELETE] );
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N�Z�b�g																				*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->DispLineBlink[PrmCopyV.SubMode+1] = TRUE;
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�u���b�N�I����ʁ@�\���f�[�^�쐬����														*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispBlockSelect(void)
{
USHORT	i, block_list_idx;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�P�s�ڂɋ@�\�ʃ^�C�g����\��																*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
						(UCHAR *)PrmCopyDispTitleTbl[PrmCopyV.SubMode+1] );
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�s�ڂ���T�s�ڂ܂ŁA�u���b�N���X�g "0X:SGDS-XXX" ��\��									*/
/*--------------------------------------------------------------------------------------------------*/
		block_list_idx = PrmCopyV.BlockNo;
		for( i = 0; i < PRMCOPY_DISP_LIST_MAX; i++ )
		{
			PrmCopyMakeBlockDispLine( TxBuf->DispLine[i+1], block_list_idx );
			if( (++block_list_idx) == PRMCOPY_MNG_INFO_BLOCK_MAX )
			{
				block_list_idx = 0;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�u���b�N���X�g�\���̂P�s��(��ʏ�͂Q�s��)���u�����N										*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->DispLineBlink[LCD_DISP_LINE_2] = TRUE;
		TxBuf->DispLineBlink[LCD_DISP_LINE_3] = 			/* @@ 2 line blink when SGD7W connected */
				Lpx_ChkDoubleBlock( PrmCopyP.TypeID ); 		
	
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		���s�J�n�m�F��ʁ@�\���f�[�^�쐬����														*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispExeConfirm(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�u���b�N�I�����ʃw�b�_�[�\��													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader();
/*--------------------------------------------------------------------------------------------------*/
/*			�������s�V�[�P���X																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		SAVE(Servo -> DgOp)���s��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SAVE:
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY(Servo <-> DgOp)���s��														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_VERIFY:
			/* �R�s�ڂ� "Start :[READ]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_START_READ] );
			/* �S�s�ڂ� "Return:[MODE]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RETURN_MODE] );
			/* �R�s�ڂ��u�����N */
			TxBuf->DispLineBlink[LCD_DISP_LINE_3] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD(DgOp -> Servo)���s��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_LOAD:
			/* �R�s�ڂ� "Start :[WRITE]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_START_WRITE] );
			/* �S�s�ڂ� "Return:[MODE]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RETURN_MODE] );
			/* �R�s�ڂ��u�����N */
			TxBuf->DispLineBlink[LCD_DISP_LINE_3] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�p�����[�^�폜(DgOp)���s��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_DELETE:
			/* �R�s�ڂ� "FILE DELETE" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_FILE_DELETE] );
			/* �S�s�ڂ� "Start :[WRITE]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_START_WRITE] );
			/* �T�s�ڂ� "Return:[MODE]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RETURN_MODE] );
			/* �S�s�ڂ��u�����N */
			TxBuf->DispLineBlink[LCD_DISP_LINE_4] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^SAVE(Servo -> DgOp)���s����ʁ@�\���f�[�^�쐬����									*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispSaveExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�u���b�N�I�����ʃw�b�_�[�\��													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader();
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ:	/* �w�b�_���폜�R�}���h�v��						*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP:	/* �w�b�_���폜�R�}���h�����m�F					*/
		case PRMCOPY_SAVE_SEQ_HEADER_SEND:		/* �w�b�_���o�^�R�}���h���M						*/
		case PRMCOPY_SAVE_SEQ_HEADER_RSP:		/* �w�b�_���o�^�R�}���h�����m�F					*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_SEND:			/* �p�����[�^�f�[�^�`���R�}���h���M					*/
		case PRMCOPY_SAVE_SEQ_PRM_RSP:			/* �p�����[�^�f�[�^�`���R�}���h�����m�F				*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_REQ:		/* @@ �p�����[�^�f�[�^EEPROM�Ǐo��					*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_WAIT:	/* @@ �p�����[�^�f�[�^EEPROM�Ǐo��					*/
		/*------------------------------------------------------------------------------------------*/
			if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_500MSEC )
			{
				/* �R�s�ڂ� "Reading" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_READING] );
				/* �S�s�ڂ� "Parameters.." ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* �T�s�ڂ� "<<SERVO>>" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SERVO] );
			}
			else if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_1000MSEC )
			{
				/* �R�s�ڂ� "Writing" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_WRITING] );
				/* �S�s�ڂ� "Parameters.." ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* �T�s�ڂ� "<< OP >>" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP] );
			}
			else
			{
				KpiRstLongTimer( &PrmCopyV.DispKeepTime );
			}
			/* [Read]�L�[ LED �_�� */
			LopV.LedBlnkSw.read = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���s������ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_COMPLETE:
			/* �S�s�ڂ� "Completed" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Read]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���f��ʕ\����																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_CANCEL:
			/* �S�s�ڂ� "Canceled" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Read]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^LOAD(DgOp -> Servo)���s����ʁ@�\���f�[�^�쐬����									*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispLoadExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�u���b�N�I�����ʃw�b�_�[�\��													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader();
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD�������A���s�O�Ċm�F����														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_RECONFIRM:
			/* �R�s�ڂ� "Reconfirm" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RECONFIRM] );
			/* �S�s�ڂ� "OK :[DATA]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OK_DATA] );
			/* �T�s�ڂ� "NO :[MODE]" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_NO_MODE] );
			/* �R�s�ڂ��u�����N */
			TxBuf->DispLineBlink[LCD_DISP_LINE_3] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/* 		�p�����[�^READ�R�}���h�v��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_REQ:
		/*------------------------------------------------------------------------------------------*/
		/* 		�p�����[�^READ�R�}���h�����m�F														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_RSP:
			if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_500MSEC )
			{
				/* �R�s�ڂ� "Reading" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_READING] );
				/* �S�s�ڂ� "Parameters.." ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* �T�s�ڂ� "<< OP >>" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP] );
			}
			else if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_1000MSEC )
			{
				/* �R�s�ڂ� "Writing" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_WRITING] );
				/* �S�s�ڂ� "Parameters.." ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* �T�s�ڂ� "<<SERVO>>" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SERVO] );
			}
			else
			{
				KpiRstLongTimer( &PrmCopyV.DispKeepTime );
			}
			/* [Write]�L�[ LED �_�� */
			LopV.LedBlnkSw.write = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���s������ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_COMPLETE:
			/* �S�s�ڂ� "Completed" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Write]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.write = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���f��ʕ\����																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_CANCEL:
			/* �S�s�ڂ� "Canceled" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Write]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.write = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		VERIFY(Servo <-> DgOp)���s����ʁ@�\���f�[�^�쐬����										*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispVerifyExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X���AVERIFY���� �s��v��ʕ\�����ȊO�̎�									*/
/*--------------------------------------------------------------------------------------------------*/
		if( PrmCopyV.ExeSeq != PRMCOPY_VERIFY_SEQ_UNMATCHED )
		{
			/* �p�����[�^�u���b�N�I�����ʃw�b�_�[�\�� */
			PrmCopyDispBlockHeader();
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/* 		�p�����[�^READ�R�}���h�v����														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_REQ:
		/*------------------------------------------------------------------------------------------*/
		/* 		�p�����[�^READ�R�}���h�����m�F��													*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_RSP:
			if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_500MSEC )
			{
				/* �R�s�ڂ� "Reading" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_READING] );
				/* �S�s�ڂ� "Parameters.." ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* �T�s�ڂ� "<< OP >>" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP] );
			}
			else if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_1000MSEC )
			{
				/* �R�s�ڂ� "Verifying" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_VERIFYING] );
				/* �S�s�ڂ� "Parameters.." ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* �T�s�ڂ� "<<OP-SERVO>>" ��\�� */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP_SERVO] );
			}
			else
			{
				KpiRstLongTimer( &PrmCopyV.DispKeepTime );
			}
			/* [Read]�L�[ LED �_�� */
			LopV.LedBlnkSw.read = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���s������ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_COMPLETE:
			/* �S�s�ڂ� "Completed" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Read]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���f��ʕ\����																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_CANCEL:
			/* �S�s�ڂ� "Canceled" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Read]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY���� ��v��ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_MATCHED:
			/* �R�s�ڂ� "All Parameters" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ALL_PARAMETERS] );
			/* �S�s�ڂ� "  are matched" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ARE_MATCHED] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY���� �s��v��ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_UNMATCHED:
			PrmCopyDispVerifyUnmatchList();
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		VERIFY���� �s��v��ʁ@�\���f�[�^�쐬����													*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispVerifyUnmatchList(void)
{
LONG	i;
LONG	PrmIdx;
LONG	list_tbl_idx;
UCHAR	*disp_str_buf;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�P�s�ڂɋ@�\�ʃ^�C�g����\��																*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
						(UCHAR *)PrmCopyDispTitleTbl[PrmCopyV.SubMode+1] );
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�s�ڂ���T�s�ڂ܂ŁA�s��v�p�����[�^NO��\��												*/
/*--------------------------------------------------------------------------------------------------*/
		list_tbl_idx = PrmCopyV.VerifyErrListNo;
		for( i = 0; i < PRMCOPY_DISP_LIST_MAX; i++, list_tbl_idx++ )
		{
			/* �\������f�[�^������ */
			if( list_tbl_idx < PrmCopyV.VerifyErrTblCnt )
			{
				disp_str_buf = TxBuf->DispLine[i+1];
				PrmIdx = PrmCopyV.VerifyErrPrmIdxTbl[list_tbl_idx];
				if( list_tbl_idx < (PRMCOPY_VERIFY_ERR_DISP_TBL_MAX - 1) )
				{
					/* "Pn"�Z�b�g */
					MlibCopyByteMemory( (UCHAR *)&PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PN][0],
										(UCHAR *)disp_str_buf, 2 );
					/* �s��v�p�����[�^���A���[�U�[���x���P�̏ꍇ */
					if( LcdPdlTbl[PrmIdx].pPrmDef->AccessLevel == ACCLVL_USER1 )
					{
						/* �p�����[�^�ԍ��Z�b�g */
						ConvertHexToStr( LcdPdlTbl[PrmIdx].PnNo, (disp_str_buf+2), 3 );
					}
					else
					{
						/* "***"�Z�b�g */
						MlibCopyByteMemory( (UCHAR *)&PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ASTER][0],
											(UCHAR *)(disp_str_buf+2), 3 );
					}
				}
				else if( list_tbl_idx == (PRMCOPY_VERIFY_ERR_DISP_TBL_MAX - 1) )
				{
					/* "To Many Errors"�Z�b�g */
					MlibCopyByteMemory( (UCHAR *)&PrmCopyDispStrTbl[PRMCOPY_DISPSTR_TO_MANY_ERRORS][0],
										(UCHAR *)disp_str_buf, 14 );
				}
			}
		/*------------------------------------------------------------------------------------------*/
			if( i == 1 )
			{
				/* �R�s�ڂ� "Unmatched" ��\�� */
				PrmCopyStrCopy( &TxBuf->DispLine[i+1][7],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_UNMATCHED_LEFT] );
			}
			else if( i == 2 )
			{
				/* �S�s�ڂ� "Parameters" ��\�� */
				PrmCopyStrCopy( &TxBuf->DispLine[i+1][7],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_LEFT] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		DELETE(DgOp)���s����ʁ@�\���f�[�^�쐬����													*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispDeleteExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�u���b�N�I�����ʃw�b�_�[�\��													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader( );
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�w�b�_���폜�R�}���h�v��															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ:
		/*------------------------------------------------------------------------------------------*/
		/*		�w�b�_���폜�R�}���h�����m�F														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP:
			/* �R�s�ڂ� "Deleting" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_DELETING] );
			/* �S�s�ڂ� "Parameters.." ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
			/* [Write]�L�[ LED �_�� */
			LopV.LedBlnkSw.write = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���s������ʕ\����															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_COMPLETE:
			/* �S�s�ڂ� "Completed" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Write]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.write = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y���f��ʕ\����																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_CANCEL:
			/* �S�s�ڂ� "Canceled" ��\�� */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Write]�L�[ LED �_�Œ�~ */
			LopV.LedBlnkSw.write = FALSE;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�u���b�N�I������ �w�b�_�[�\���f�[�^�쐬����										*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispBlockHeader( void )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�P�s�ڂɋ@�\�ʃ^�C�g����\��																*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
						(UCHAR *)PrmCopyDispTitleTbl[PrmCopyV.SubMode+1] );
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�s�ڂɎ��s�Ώۃu���b�N "0X:SGDS-XXX" ��\��												*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyMakeBlockDispLine( TxBuf->DispLine[LCD_DISP_LINE_2], PrmCopyV.BlockNo );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y������ʁ@�\���f�[�^�쐬����														*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispInit( void )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�Q�s�ڂ� "File List Loading" ��\��															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_2],
						(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_FILE_LIST_LOADING] );
/*--------------------------------------------------------------------------------------------------*/
/*		�R�s�ڂ� "Please Wait..." ��\��															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
						(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PLEASE_WAIT] );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�u���b�N�\���f�[�^�쐬����																	*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyMakeBlockDispLine( UCHAR *StrBuf, LONG BlockIdx )
{
LONG	i;
LONG	ServoId;

/*--------------------------------------------------------------------------------------------------*/
/*		���C���N���A																				*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SPACE] );
/*--------------------------------------------------------------------------------------------------*/
/*		�u���b�NNO�Z�b�g																			*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispBlockStrTbl[ BlockIdx ] );
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�p�b�N�A���}�b�`�̏ꍇ : "****"�Z�b�g													*/
/*--------------------------------------------------------------------------------------------------*/
		if( PrmMngTbl[BlockIdx].FormatID == PRMCOPY_MNG_DATA_NONE )
		{
			PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ASTER] );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�p�b�N���i���Z�b�g(��"SGDS-")															*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < SVIDTBL_ENTRY_NUM; i++ )
		{
			if( (PrmMngTbl[BlockIdx].ServoID & 0x00FF) == SvIdTbl[i].Id )
			{
				ServoId = i;	/* �A���v�e�ʕ\��������Z�b�g�Ŏg�p*/
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)&SvIdTbl[i].Name );
				break;
			}
			else if( i >= (SVIDTBL_ENTRY_NUM - 1) )		/* �����s��v : "????"�Z�b�g			*/
			{
				ServoId = i;
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_4] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		"-" �Z�b�g																					*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_HIFUN] );
/*--------------------------------------------------------------------------------------------------*/
/*		�A���v�e�ʕ\��������Z�b�g																	*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < AMP_WATT_TBL_IDX_MAX; i++ )
		{
			if(( (PrmMngTbl[BlockIdx].AmpVolt & 0x000f) == AmpWattTbl[i].AmpVolt ) &&
			   ( PrmMngTbl[BlockIdx].AmpWatt == AmpWattTbl[i].AmpWatt ))
			{
				/*----------------------------------------------------------------------------------*/
				/*	���i�ɂ��[W]�\����[Arms]�L���\���𕪂��� 										*/
				/*----------------------------------------------------------------------------------*/
				if( SvIdTbl[ServoId].Disp == WATT)			/* [W]�\��								*/
				{
					PrmCopyStrCopy2( &StrBuf, (UCHAR *)&AmpWattTbl[i].DispStrW[0] );
				}
				else if( SvIdTbl[ServoId].Disp == AMPERE)	/* [Arms]�L���\��						*/
				{
					PrmCopyStrCopy2( &StrBuf, (UCHAR *)&AmpWattTbl[i].DispStr[0] );
				}
				else										/* ���̑��F"???"�Z�b�g					*/
				{
					PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_3] );
				}
				break;
			}
			else if( i >= (AMP_WATT_TBL_IDX_MAX - 1) )		/* �����s��v : "???"�Z�b�g				*/
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_3] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���͓d���\��������Z�b�g																	*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < AMP_VOLT_TBL_IDX_MAX; i++ )
		{
			if( (PrmMngTbl[BlockIdx].AmpVolt & 0x000f) == AmpVoltTbl[i].Code )
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)&AmpVoltTbl[i].DispStr[0] );
				break;
			}
			else if( i >= (AMP_VOLT_TBL_IDX_MAX - 1) )		/* �����s��v : "?"�Z�b�g				*/
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_1] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�C���^�[�t�F�C�X�d�l�\��������Z�b�g														*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < SVIDTBL_ENTRY_NUM; i++ )
		{
			if( (PrmMngTbl[BlockIdx].ServoID & 0x00FF) == SvIdTbl[i].Id )
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)&SvIdTbl[i].IfTyp );
				break;
			}
			else if( i >= (SVIDTBL_ENTRY_NUM - 1) )		/* �����s��v : "????"�Z�b�g			*/
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_4] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		������R�s�[����																			*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyStrCopy( UCHAR *ToStr, UCHAR *FromStr )
{
		while( *FromStr != 0 )
		{
			*(ToStr++) = *(FromStr++);
		}
}



/****************************************************************************************************/
/*																									*/
/*		������R�s�[����(�R�s�[��̃A�h���X���X�V���ĕԂ�)											*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyStrCopy2( UCHAR **ToStr, UCHAR *FromStr )
{
		while( *FromStr != 0)
		{
			*((*ToStr)++) = *(FromStr++);
		}
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�p�����[�^�u���b�N�Ǘ����擾����(�f�W�I�y�ڑ����ɂP��s��)						*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdPrmInfoGet( void )
{
LONG	i;
SHORT	rc;
LONG	ModeChg = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyCmdInit();						/* �f�W�I�y�ʐM��������								*/
		PrmCopyNormalCmdSet();					/* �ʐM��Ɨ̈�ɁA�ʏ�R�}���h(0x60)�Z�b�g			*/
		PrmCopyDispInit();						/* �f�W�I�y������ʕ\���f�[�^�Z�b�g					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�������s�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		���s��������																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_INIT:
			/* �ʐM���g���C�J�E���g�N���A */
			PrmCopyV.RetryCnt = 0;
			PrmCopyV.BlockNo = 0;
			PrmCopyV.BlockMngAreaNo = 0;
			/* �p�����[�^�u���b�N�Ǘ����READ�R�}���h�v���� */
			PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_REQ;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�f�W�I�y�p�����[�^�u���b�N�Ǘ����LOAD�R�}���h�v��									*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_MNG_READ_REQ:
			/* �ʐM��Ɨ̈��LOAD�R�}���h(0x61)�Z�b�g */
			PrmCopyLoadMngInfoCmdSet( PrmCopyV.BlockNo, PrmCopyV.BlockMngAreaNo );
			/* �p�����[�^�u���b�N�Ǘ����READ�R�}���h�����m�F�� */
			PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�R�}���h�����m�F																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_MNG_READ_RSP:
			/* �f�W�I�y�ʐM�R�}���h�����m�F���� */
			rc = PrmCopyCmdRspCheck( LCD_CMD_LOAD );
			/*----- ��M���� -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* �ʐM���g���C�J�E���g�N���A */
				PrmCopyV.RetryCnt = 0;
				/* �f�W�I�y �p�����[�^�u���b�N�Ǘ�����M�f�[�^�擾 */
				PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_RECIEVE, PrmCopyV.BlockNo );
			  /*------------------------------------------------------------------------------------*/
				if( PrmCopyV.BlockMngAreaNo == 0 )
				{
					PrmCopyV.BlockMngAreaNo = 1;
				}
				else
				{
					PrmCopyV.BlockNo++;
					PrmCopyV.BlockMngAreaNo = 0;
				}
			  /*------------------------------------------------------------------------------------*/
				/* �p�����[�^�Ǘ�����S�Ď擾���� */
				if( PrmCopyV.BlockNo >= PRMCOPY_MNG_INFO_BLOCK_MAX )
				{
					/* �Ǘ����擾������ */
					PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_COMPLETE;
				}
				else
				{
					/* �����������̃f�[�^�`�����s�� */
					PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_REQ;
				}
			}
			/*----- �A���[���R�[�h����M���� -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* �f�W�I�y�p�����[�^�u���b�N�Ǘ����READ�R�}���h���M�֖߂� */
				PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_REQ;
			}
			/*----- ���g���C�񐔃I�[�o�[ -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* �f�W�I�y �p�����[�^�u���b�N�Ǘ����N���A */
				for( i = 0; i < PRMCOPY_MNG_INFO_BLOCK_MAX; i++ )
				{
					PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_CLEAR, i );
				}
				/* �������f�� */
				PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_CANCEL;
			}
			/*----- ��M�����R�}���h�R�[�h�����M�����R�[�h�ƈႤ -----------------------------------*/
			else
			{
				/* ����������M�҂� */
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		���s����																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_COMPLETE:
		/*------------------------------------------------------------------------------------------*/
		/*		���s���f																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_CANCEL:
			ModeChg = TRUE;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�I������																					*/
/*--------------------------------------------------------------------------------------------------*/
	 	PrmCopySetSendBuf();		/* �ʐM��Ɨ̈悩��A���M�o�b�t�@�փf�[�^�Z�b�g					*/
		PrmEntNum.SvPrm = LCDPDLTBL_ENTNUM; /* PrmCopyGetEntNum( &PnPrmTbl[0], KsysP.PnPrmTblEntNum ); */
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM�R�}���h�����m�F����															*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyCmdRspCheck( USHORT SendCmd )
{
UCHAR	RecvCmd = PRMCOPY_CMD_CODE_GET( );				/* ��M�R�}���h�R�[�h�擾					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Check Receive Cmd																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RecvCmd & LCD_RCVCMD_MASK) != SendCmd )	/* ��M�R�}���h != ���M�R�}���h				*/
		{
			return( RC_PASS );
		}
		else if( (RecvCmd & LCD_RCVCMD_ALARM_BIT) )		/* �A���[���r�b�g�������Ă���				*/
		{
			if( PrmCopyV.RetryCnt > RETRY_CNT_MAX )
			{
				return( RC_RETRY_OVER );				/* ���g���C�񐔂��A���e�͈͂��z����			*/
			}
			else
			{
				PrmCopyV.RetryCnt++;					/* �ʐM���g���C�J�E���g�C���N�������g		*/
			}
			return( RC_NG );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );								/* ����										*/
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�p�����[�^�Ǘ���񑀍�																*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyPrmMngTblOp( UCHAR flag, USHORT block_no )
{
PRMDATA *pPrm = LopMsg.pAxRsc->UniPrms->Prm;			/* TODO: ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		Switch by flag																				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( flag )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�Ǘ����N���A																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MNG_INFODATA_CLEAR:
			PrmMngTbl[block_no].ServoID		= PRMCOPY_MNG_DATA_NONE;
			PrmMngTbl[block_no].AmpWatt		= PRMCOPY_MNG_DATA_NONE;
			PrmMngTbl[block_no].AmpVolt		= PRMCOPY_MNG_DATA_NONE;
			PrmMngTbl[block_no].FormatID 	= PRMCOPY_MNG_DATA_NONE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�Ǘ����Z�b�g																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MNG_INFODATA_SET:
			PrmMngTbl[block_no].ServoID		= PrmCopyP.TypeID;
			PrmMngTbl[block_no].AmpWatt		= pPrm->ampw;
			PrmMngTbl[block_no].AmpVolt		= (USHORT)pPrm->styp.b.h;
			PrmMngTbl[block_no].FormatID 	= EEP_FORMAT_ID; /* EepInfo.YecFmtId; */
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�Ǘ�����M�f�[�^�擾																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MNG_INFODATA_RECIEVE:
			if( PrmCopyV.BlockMngAreaNo == 0 )
			{
				PrmMngTbl[block_no].ServoID  = 0xFFFF;
				PrmMngTbl[block_no].AmpWatt  = PRMCOPY_CMD_DATA_GET( 0 );
				PrmMngTbl[block_no].AmpVolt  = PRMCOPY_CMD_DATA_GET( 1 );
				PrmMngTbl[block_no].FormatID = PRMCOPY_CMD_DATA_GET( 2 );
			/*------------------------------------------------------------------------------------*/
				if( PrmMngTbl[block_no].AmpWatt != 0xFFFF )
				{
					PrmMngTbl[block_no].ServoID = SVID_SGDS;
				}
			}
			else if( PrmMngTbl[block_no].ServoID == 0xFFFF )
			{
				if( PrmMngTbl[block_no].FormatID != 0xFFFF )
				{
					PrmMngTbl[block_no].ServoID  = PRMCOPY_CMD_DATA_GET( 0 );
					PrmMngTbl[block_no].AmpWatt  = PRMCOPY_CMD_DATA_GET( 1 );
					PrmMngTbl[block_no].AmpVolt  = PRMCOPY_CMD_DATA_GET( 2 );
					PrmMngTbl[block_no].FormatID = PRMCOPY_CMD_DATA_GET( 3 );
				}
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM����������																		*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyCmdInit( void )
{
LONG	i;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		���M�o�b�t�@�N���A																			*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)LopMsg.ResBuf, sizeof(PRM_COPY_TXBUF) );
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM��Ɨ̈揉��������																*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PRMCOPY_DISP_LINE_MAX; i++ )
		{
			PrmCopyStrCopy( TxBuf->DispLine[i], (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SPACE] );
			TxBuf->DispLineBlink[i] = FALSE;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM  �ʏ�R�}���h(0x60)��Ɨ̈�Z�b�g												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyNormalCmdSet( void )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;
#define LCD_CMD_NORMAL_MSGLEN  LCD_TX_NORMAL_TOTAL_SIZE + LCD_MSG_CHKSUM_SIZE

/*--------------------------------------------------------------------------------------------------*/
/*		�w�b�_�ݒ�																					*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_NOR;				/* �ʏ�R�}���h(0x60)�Z�b�g					*/
		TxBuf->Length  = LCD_CMD_NORMAL_MSGLEN;			/* ���M�o�b�t�@���Z�b�g						*/
		TxBuf->DispSts[0] = 5;							/* �\���`���Z�b�g							*/
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM  LOAD�R�}���h(0x61)��Ɨ̈�Z�b�g												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadCmdSet( USHORT DopEepAddr, USHORT DataNum )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;
#define LCD_CMD_LOAD_MSGLEN  LCD_TX_MSG_LOADSUM_IDX + LCD_MSG_CHKSUM_SIZE

/*--------------------------------------------------------------------------------------------------*/
/*		�w�b�_�ݒ�																					*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_LOAD;					/* LOAD�R�}���h(0x61)�Z�b�g 				*/
		TxBuf->Length  = LCD_CMD_LOAD_MSGLEN;			/* ���M���b�Z�[�W���Z�b�g					*/
		TxBuf->EepAddr = DopEepAddr;					/* �f�W�I�yEEPROM�A�h���X�Z�b�g 			*/
		TxBuf->DataNum = DataNum;						/* �f�[�^���Z�b�g 							*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM  SAVE�R�}���h(0x62)��Ɨ̈�Z�b�g												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveCmdSet( USHORT DopEepAddr, USHORT DataNum, USHORT *pData )
{
LONG	i;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;
#define LCD_CMD_SAVE_DATLEN  (DataNum * LCD_MSG_DATA_UNIT_SIZE)
#define LCD_CMD_SAVE_MSGLEN  LCD_TX_MSG_DATA_IDX + LCD_MSG_CHKSUM_SIZE + LCD_CMD_SAVE_DATLEN

/*--------------------------------------------------------------------------------------------------*/
/*		�w�b�_�ݒ�																					*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_SAVE;					/* SAVE�R�}���h(0x62)�Z�b�g 				*/
		TxBuf->Length  = LCD_CMD_SAVE_MSGLEN;			/* ���M���b�Z�[�W���Z�b�g					*/
		TxBuf->EepAddr = DopEepAddr;					/* �f�W�I�yEEPROM�A�h���X�Z�b�g 			*/
		TxBuf->DataNum = DataNum;						/* �f�[�^���Z�b�g 							*/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�[�^�Z�b�g(MAX:8)																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < DataNum; i++ )
		{
			TxBuf->Data[i] = *(pData++);
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM : �Ǘ���񃍁[�h�R�}���h�f�[�^�ݒ�												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadMngInfoCmdSet( LONG BlockNo, LONG BlockMngAreaNo )
{
USHORT			DataNum, EepSize, EepBase;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�Ǘ����̈�O : �����Ǘ����(SGDS�݊�)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( BlockMngAreaNo == 0 )
		{
			DataNum = 3;										/* ���[�h�f�[�^��					*/
			EepSize = PRMCOPY_MNG_INFO_SIZ0;					/* EEPROM�Ǘ����T�C�Y				*/
			EepBase = PRMCOPY_MNG_INFO_TOP0;					/* EEPROM�x�[�X�A�h���X				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�Ǘ����̈�P�F�g���Ǘ����(SVOS�ǉ�)														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			DataNum = 8;										/* ���[�h�f�[�^��					*/
			EepSize = PRMCOPY_MNG_INFO_SIZ1;					/* EEPROM�Ǘ����T�C�Y				*/
			EepBase = PRMCOPY_MNG_INFO_TOP1;					/* EEPROM�x�[�X�A�h���X				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���M�o�b�t�@�f�[�^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_LOAD;							/* ���[�h�R�}���h�Z�b�g				*/
		TxBuf->EepAddr = (USHORT)(EepBase + (EepSize*BlockNo));	/* EEPROM�A�h���X�Z�b�g				*/
		TxBuf->DataNum = DataNum;								/* ���[�h�f�[�^���Z�b�g				*/
		TxBuf->Length  = LCD_TX_MSG_LOADSUM_IDX + LCD_MSG_CHKSUM_SIZE;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�f�W�I�y�ʐM : �Ǘ����ۑ��R�}���h�f�[�^�ݒ�												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveMngInfoCmdSet( LONG BlockNo, LONG BlockMngAreaNo )
{
USHORT			DataNum, EepSize, EepBase;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�Ǘ����̈�O : �����Ǘ����(SGDS�݊�)														*/
/*--------------------------------------------------------------------------------------------------*/
/*		���ӁFSGDS���A"???"��\������悤�ɁAAmpWatt,AmpVolt�ɂ́A0xFFFF����������					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BlockMngAreaNo == 0 )
		{
			DataNum = 3;										/* �ۑ��f�[�^��						*/
			EepSize = PRMCOPY_MNG_INFO_SIZ0;					/* EEPROM�Ǘ����T�C�Y				*/
			EepBase = PRMCOPY_MNG_INFO_TOP0;					/* EEPROM�x�[�X�A�h���X				*/
		/*------------------------------------------------------------------------------------------*/
			TxBuf->Data[0] = 0xFFFF;							/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[1] = 0xFFFF;							/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[2] = PrmMngTbl[BlockNo].FormatID;		/* �f�[�^�Z�b�g(MAX:8)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�Ǘ����̈�P�F�g���Ǘ����(SVOS�ǉ�)														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			DataNum = 8;										/* �ۑ��f�[�^��						*/
			EepSize = PRMCOPY_MNG_INFO_SIZ1;					/* EEPROM�Ǘ����T�C�Y				*/
			EepBase = PRMCOPY_MNG_INFO_TOP1;					/* EEPROM�x�[�X�A�h���X				*/
		/*------------------------------------------------------------------------------------------*/
			TxBuf->Data[0] = PrmMngTbl[BlockNo].ServoID;		/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[1] = PrmMngTbl[BlockNo].AmpWatt;		/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[2] = PrmMngTbl[BlockNo].AmpVolt;		/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[3] = PrmMngTbl[BlockNo].FormatID;		/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[4] = 0xFFFF;							/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[5] = 0xFFFF;							/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[6] = 0xFFFF;							/* �f�[�^�Z�b�g(MAX:8)				*/
			TxBuf->Data[7] = 0xFFFF;							/* �f�[�^�Z�b�g(MAX:8)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���M�o�b�t�@�f�[�^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_SAVE;							/* �ۑ��R�}���h�Z�b�g				*/
		TxBuf->EepAddr = (USHORT)(EepBase + (EepSize*BlockNo));	/* EEPROM�A�h���X�Z�b�g				*/
		TxBuf->DataNum = DataNum;								/* �ۑ��f�[�^���Z�b�g				*/
		TxBuf->Length  = LCD_TX_MSG_DATA_IDX
						 + (DataNum * LCD_MSG_DATA_UNIT_SIZE) + LCD_MSG_CHKSUM_SIZE;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�ʐM��Ɨ̈悩��A���M�o�b�t�@�փf�[�^�Z�b�g����											*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySetSendBuf( void )
{
LONG	i;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		�R�}���h�Z�b�g																				*/
/*--------------------------------------------------------------------------------------------------*/
		LopV.CurMsgCmd = TxBuf->CmdCode;
/*--------------------------------------------------------------------------------------------------*/
/*		��ʕ\���f�[�^�Z�b�g																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PRMCOPY_DISP_LINE_MAX; i++ )
		{
			if( TxBuf->DispLineBlink[i] )
			{
				RpxLcdSetBlnk( i, 0, LCD_DISP_LINE_CHAR_MAX, (UCHAR *)TxBuf );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����f�[�^�G���f�B�A���ϊ� (TxBuf->Length/ChkSum�́ALcdOp.c�ŏ���)							*/
/*--------------------------------------------------------------------------------------------------*/
#if		defined( CCSW_BIG_ENDIAN )
		switch( TxBuf->CmdCode )
		{
		case LCD_CMD_LOAD:	/* �f�W�I�y EEPROM�Ǐo��	*/
			 TxBuf->EepAddr = MlibCnvLittle16( TxBuf->EepAddr );
			 TxBuf->DataNum = MlibCnvLittle16( TxBuf->DataNum );
			 break;
		case LCD_CMD_SAVE:	/* �f�W�I�y EEPROM������	*/
			 for( i=0; i < TxBuf->DataNum; i++ )
			 {
				TxBuf->Data[i] = MlibCnvLittle16( TxBuf->Data[i] );
			 }
		   /*---------------------------------------------------------------------------------------*/
			 TxBuf->EepAddr = MlibCnvLittle16( TxBuf->EepAddr );
			 TxBuf->DataNum = MlibCnvLittle16( TxBuf->DataNum );
			 break;
		default:
			 break;
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�e�[�u������A���[�U���x���̓o�^�����Z�o											*/
/*																									*/
/****************************************************************************************************/
#if 0
USHORT	PrmCopyGetEntNum( const PRMTBL *PrmTbl, const USHORT EntNum )
{
LONG	i;
CPRMDEF	*pPrmDef;
USHORT	PrmNum = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		Count User Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i=0; i < EntNum; i++ )
		{
			pPrmDef = PrmTbl[i].PrmDefPtr;
			if(	LPX_CHK_EEP_PRM( pPrmDef ) )
			{
				ULONG	AccLvl = pPrmDef->AccessLevel;
				if( (AccLvl == ACCLVL_USER1) || (AccLvl == ACCLVL_USER2) )
				{
					if( pPrmDef->Attr.VarIsLong ){ PrmNum += 2;}else{ PrmNum += 1;}
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( PrmNum );
}
#endif



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^EEPROM�ǂݏo������																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v: �w�肳���p�����[�^��EEPROM����WORD�T�C�Y1�ǂݏo���B									*/
/*		  Reads the specified Parameter value from EEPROM.											*/
/*	����: ��ʂ���̃T�C�Y�w��͂Ȃ��B�p�����[�^�R�s�[�@�\��p����									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	PrmCopyPrmRead( USHORT Index, const PRMTBL *PrmTbl, const USHORT EntNum, USHORT *ResData )
{
ULONG	PrmData;
CPRMDEF *pPrmDef;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= (2 * EntNum) )
		{
			return( PERR_NO );				/* �p�����[�^�ԍ��G���[ */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM�f�[�^�ǂݏo��																		*/
/*--------------------------------------------------------------------------------------------------*/
		pPrmDef = LPX_GET_PrmDefPtr( Index );
		PrmData = KriReadEepromPrm( pPrmDef );
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM�Ǐo�f�[�^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( pPrmDef->Attr.VarIsLong != TRUE )
		{
			*ResData = (USHORT)PrmData;
		}
		else if( LPX_GET_HiWdFlag( Index ) == 0 )
		{
			*ResData = GET_LWD_EEPDATA( PrmData );
		}
		else
		{
			*ResData = GET_HWD_EEPDATA( PrmData );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif







/***************************************** end of file **********************************************/
