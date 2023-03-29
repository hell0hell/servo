/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SvIdDef.h : Servo ID Definition Common Header File											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. �T�[�{�@��h�e��`																		*/
/*																									*/
/*		2. 																							*/
/*																									*/
/*		3. 																							*/
/*																									*/
/*		4. 																							*/
/*																									*/
/*		5. 																							*/
/*																									*/
/*		6. 																							*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.10.18  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___SERVOID___
#define ___SERVOID___


/****************************************************************************************************/
/*																									*/
/*		���i�p�h�c��`(�O���d�l�F�ύX�s��)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�� �� : �T�[�{�@��h�c�ɂ�鏈�����h�e�́A�\�Ȍ���A�쐬���Ȃ����ƁB							*/
/*			�T�[�{�@�킪������x�ɁA�������h�e�̒ǉ����K�v�ł���A������j�]����I�I�I				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�@��h�c��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define SVID_SGDS				0x0014					/* SGDS�F�W���^								*/
#define SVID_SGDS_FC			0x0015					/* SGDS�F�e�b�^								*/
/*--------------------------------------------------------------------------------------------------*/
#define	SVID_SJDE				0x0021					/* SJDE�F�W���^								*/
/*--------------------------------------------------------------------------------------------------*/
#define SVID_SGDX				0x0024					/* SGDX�F�W���^								*/
#define SVID_SGDX_FC			0x0025					/* SGDX�F�e�b�^								*/
/*--------------------------------------------------------------------------------------------------*/
#define SVID_SGDV_R				0x0028					/* SGDV�F�W���^(��۸�/��ٽ��`)				*/
#define SVID_SGDV_RM2			0x0029					/* SGDV�F�W���^(MECHATROLINK�`)				*/
#define SVID_SGDV_ROP			0x002A					/* SGDV�F�W���^(�w�ߵ�߼�ݑΉ��`)			*/
#define SVID_SGDV_L				0x002B					/* SGDV�F���j�A�^(��۸�/��ٽ��`)			*/
#define SVID_SGDV_LM2			0x002C					/* SGDV�F���j�A�^(MECHATROLINK�`)			*/
#define SVID_SGDV_LOP			0x002D					/* SGDV�F���j�A�^(�w�ߵ�߼�ݑΉ��`)			*/
#define SVID_SGDW_R				0x0030					/* SGDW�F�W���^(��۸�/��ٽ��`)				*/
#define SVID_SGDW_RM2			0x0031					/* SGDW�F�W���^(MECHATROLINK�`)				*/
#define SVID_SGDW_L				0x0032					/* SGDW�F���j�A�^(��۸�/��ٽ��`)			*/
#define SVID_SGDW_LM2			0x0033					/* SGDW�F���j�A�^(MECHATROLINK�`)			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SVID_IDM				0x0040					/* IDM �F�l�b�g���[�N�^						*/
#define	SVID_IDM35				SVID_IDM				/* IDM �F35mm								*/
#define	SVID_IDM50				SVID_IDM				/* IDM �F50mm								*/
#define	SVID_IDM60				SVID_IDM				/* IDM �F60mm								*/
/*--------------------------------------------------------------------------------------------------*/
#define	SVID_CACR_JUS_M3		0x0050					/* CACR-JU:�P��(MECHATROLINK3�`)			*/
#define	SVID_CACR_JUM_M3		0x0051					/* CACR-JU:����(MECHATROLINK3�`)			*/
#define	SVID_CACR_JUS_COE		0x0052					/* CACR-JU:�P��(COE�`)						*/
#define	SVID_CACR_JUM_COE		0x0053					/* CACR-JU:����(COE�`)						*/
#define	SVID_CACR_JUS_AN		0x005C					/* CACR-JU:�P��(Analog�`)					*/
/*--------------------------------------------------------------------------------------------------*/
#define SVID_SGD7S_AN			0x0070					/* SGD7S�F��۸�/��ٽ��w�ߌ`				*/
#define SVID_SGD7S_M2			0x0071					/* SGD7S�FMECHATROLINK-II�ʐM�w�ߌ`			*/
#define SVID_SGD7S_M3			0x0072					/* SGD7S�FMECHATROLINK-III�ʐM�w�ߌ`		*/
#define SVID_SGD7S_OP			0x0073					/* SGD7S�F�w�ߵ�߼�ݎ��t���`				*/
#define SVID_SGD7S_COE			0x0074					/* SGD7S�FEtherCAT�ʐM�w�ߌ`				*/
#define SVID_SGD7W_M3			0x0075					/* SGD7W�FMECHATROLINK-III�ʐM�w�ߌ`		*/
/*--------------------------------------------------------------------------------------------------*/
#define	SVID_SGDA				0xFFFF					/* SGDA�F�W���^		@CHK@�F�����u			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�l�T�[�{�@��h�c(�T�[�{�n�r�ł́A���ݕs�g�p)												*/
/*--------------------------------------------------------------------------------------------------*/
#define	SVID_SGDM				0x0001					/* SGDM�F�W���^		(�Q�l,Wcmd-IF)			*/
#define	SVID_SGDH				0x0002					/* SGDH�F�W���^		(�Q�l,Wcmd-IF)			*/
#define	SVID_SGDH_LM			0x0004					/* SGDH�F���j�A�^	(�Q�l,Wcmd-IF)			*/
#define	SVID_IDM_M4				0x0016					/* IDM �FM4,IRIS	(�Q�l,Smsg-IF)			*/
/*--------------------------------------------------------------------------------------------------*/
#define SVID_SGDV_MD_M3			0x005E					/* SGDV-MD�FMECHATROLINK-III�w�ߌ`(�Q�l)	*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		�T�[�{�@��h�c�e�[�u����`�F�k�b�c�I�y���[�^�p�����[�^�R�s�[�p								*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�� �� : �k�b�c�I�y���[�^���g�p����T�[�{�@��h�c��ǉ������ꍇ�́A								*/
/*			�{�h�c�e�[�u����`�ɂ��A�T�[�{�@��h�c�Ɩ��̂�ǉ����邱�ƁB							*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�K�p�T�[�{���[�^�o�͕\����`																*/
/*--------------------------------------------------------------------------------------------------*/
#define	WATT				0							/* [�v]�\��									*/
#define	AMPERE				1							/* [Arms]�L���\��							*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct
{
	USHORT	Id;											/* �T�[�{�@��h�c							*/
	UCHAR	Name[10];									/* �T�[�{�@�햼��(�\���p,etc)				*/
	UCHAR	IfTyp[6];									/* �C���^�[�t�F�C�X����						*/
	UCHAR	Disp;										/* �K�p���[�^�o�͕\��						*/
} SVIDTBL;
/*--------------------------------------------------------------------------------------------------*/
#define	SVIDTBL_ENTRY_DEF								/* SVIDTBL Entry Definition					*/\
	{	SVID_SGDS,		"SGDS",		"\0",	WATT	},		/* SGDS Standard   Type					*/\
	{	SVID_SGDS_FC,	"SGDS_FC",	"\0",	WATT	},		/* SGDS FullClosed Type					*/\
	{	SVID_SGDX,		"SGDX",		"01A",	WATT	},		/* SGDX Standard   Type					*/\
	{	SVID_SGDX_FC,	"SGDX",		"02A",	WATT	},		/* SGDX FullClosed Type					*/\
	{	SVID_SJDE,		"SJDE",		"NA",	WATT	},		/* SJDE Standard   Type					*/\
	{	SVID_IDM,		"IDM",		"N"	,	WATT	},		/* IDM  Network    Type					*/\
	{	SVID_SGDV_R,	"SGDV",		"01A",	AMPERE	},		/* SGDV Standard   Type					*/\
	{	SVID_SGDV_L,	"SGDV",		"05A",	AMPERE	},		/* SGDV Linear     Type					*/\
	{	SVID_SGDV_RM2,	"SGDV",		"11A",	AMPERE	},		/* SGDV M-�U Standard Type				*/\
	{	SVID_SGDV_LM2,	"SGDV",		"15A",	AMPERE	},		/* SGDV M-�U Linear   Type				*/\
	{	SVID_SGDW_R,	"SGDW",		"01A",	AMPERE	},		/* SGDW Standard      Type				*/\
	{	SVID_SGDW_L,	"SGDW",		"05A",	AMPERE	},		/* SGDW Linear        Type				*/\
	{	SVID_SGDW_RM2,	"SGDW",		"11A",	AMPERE	},		/* SGDW M-�U Standard Type				*/\
	{	SVID_SGDW_LM2,	"SGDW",		"15A",	AMPERE	},		/* SGDW M-�U Linear   Type				*/\
	{	SVID_SGD7S_AN,	"SGD7S",	"00",	AMPERE	},		/* SGD7S Analog/Pulse Type				*/\
	{	SVID_SGD7S_M2,	"SGD7S",	"10",	AMPERE	},		/* SGD7S M-II Type						*/\
	{	SVID_SGD7S_M3,	"SGD7S",	"20",	AMPERE	},		/* SGD7S M-III Type						*/\
	{	SVID_SGD7S_OP,	"SGD7S",	"E0",	AMPERE	},		/* SGD7S Command Option Type			*/\
	{	SVID_SGD7S_COE,	"SGD7S",	"A0",	AMPERE	},		/* SGD7S EtherCAT Type					*/\
	{	SVID_SGD7W_M3,	"SGD7W",	"20",	AMPERE	},		/* SGD7W M-III Type						*/\
	{	SVID_SGDV_MD_M3,"SGDV_MD",	"\0",	AMPERE	},		/* SGDV-MD M-III Type (��ŏ���)		*/\


/*--------------------------------------------------------------------------------------------------*/

#endif
/***************************************** end of file **********************************************/
