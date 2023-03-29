/****************************************************************************
Description	: Definitions for each product functionality
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

----------------------------------------------------------------------------
Changes :
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.08.06	created

******************************************************************************/
#ifndef _SYSTEM_SETTING_H
#define _SYSTEM_SETTING_H

#include "SpecCommon.h"

#ifdef _TEST_BOARD_
#define _JL076_USE_
#endif

/*****************************************************************************/
/*		Product information													 */
/*****************************************************************************/
#define MAX_AXIS_NUMBER		1			/* 譛�､ｧ謗･邯夊ｻｸ謨ｰ */
#define MAX_SLOT_NUMBER		1			/* 譛�､ｧ繧ｹ繝ｭ繝�ヨ(ASIC)謨ｰ */

#define	CSW_NETIF_TYPE		NETIF_M2_TYPE
#define CSW_BOARD_TYPE		SGD7S_TYPE										   /* <S0A2> */


/****************************************************************************/
/*		讖溽ｨｮ萓晏ｭ路/W螳夂ｾｩ														*/
/****************************************************************************/
#define SVFSEL_EEPROM_USE 	(1)			/* 1: EEPROM菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ			*/
#define SVFSEL_VIRSIF_USE	(0)			/* 1: 繝代ロ繝ｫ菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ			*/
#define SVFSEL_PANELOP_USE	(0)			/* 1: 繝代ロ繝ｫ菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ			*/
#define EEPROM_SIZE			0x1000		/* EEPROM Memory Size				*//* <S10A> */


/*****************************************************************************/
/*		繝�ヰ繝�げ逕ｨ繧ｳ繝ｳ繝代う繝ｫSW螳夂ｾｩ											 */
/*****************************************************************************/
#if !defined(CSW_CSTR_MBB_TYPE) && defined(CSW_BSP_CSTR_IFM3)
#define CSW_CSTR_DB_REVERSE		TRUE
#endif


#define CSW_CSTR_IF_PROTO_USE		FALSE
#define CSW_CSTR_AUTO_ADDR_CFG		FALSE	/* MDINIT=ON譎ゅ�閾ｪ蜍戊ｻｸ繧｢繝峨Ξ繧ｹ蜑ｲ繧贋ｻ倥¢譛臥┌ */
#define CSW_DEBUG_PRINT_EBABLE		FALSE
#define CSW_DX_CONVERTER_USE		FALSE
#define CSW_DIF_USE					FALSE
#ifdef _VC_DEBUG_
//#define CSW_NO_SERVO_ASICS
#define CSW_NO_M3_ASICS
#else
//#define CSW_NO_SERVO_ASICS
#endif

//#ifdef _IF_BOARD_REVC_	/* <S063> */
//#define	_USB_FUNC_USE_		/* revC蝓ｺ譚ｿ縺九ｉUSB讖溯�譛牙柑(繧ｽ繝輔ヨ縺ｧ蛻､譁ｭ縺ｧ縺阪↑縺�◆繧√さ繝ｳ繝代う繝ｫ繧ｹ繧､繝�メ縺ｨ縺吶ｋ) */
//#endif	//#ifdef _IF_BOARD_REVC_	/* <S063> */



#define	CSW_SCANA_ENABLE				/* CPU ScanA蜃ｦ逅�怏蜉ｹ */


/****************************************************************************/
/*		繧ｵ繝ｼ繝懈ｩ溯�驕ｸ謚槭せ繧､繝�メ(CSW_SVCF_****)								*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/*		髮ｻ蟄舌ぐ繧｢菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�蜃ｦ逅�凾髢灘ｯｾ遲�								*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_EGEAR_USE			TRUE
/*--------------------------------------------------------------------------*/
/*		繝｢繝�Ν霑ｽ蠕灘宛蠕｡菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_MFC_USE			TRUE
/*--------------------------------------------------------------------------*/
/*		A蝙句宛謖ｯ蛻ｶ蠕｡菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�										*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_ATYPE_RES_VIB_USE	TRUE
/*--------------------------------------------------------------------------*/
/*		謖ｯ蜍墓､懷�菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�										*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_VIB_DETECT_USE		TRUE
/*--------------------------------------------------------------------------*/
/*		謖ｯ蜍墓椛蛻ｶ繝輔ぅ繝ｫ繧ｿ菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�								*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_RESVIB_FIL_USE		TRUE
/*--------------------------------------------------------------------------*/
/*		蠑ｱ繧∫阜逎∝宛蠕｡菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_WEAKENFIELD_USE	TRUE
/*--------------------------------------------------------------------------*/
/*		繝槭う繧ｯ繝ｭ髮ｻ蝨ｧ陬懷─繧ｲ繧､繝ｳ讖溯�菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�						*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_MPKVVCOMP			TRUE
/*--------------------------------------------------------------------------*/
/*		螟紋ｹｱ繧ｪ繝悶じ繝ｼ繝蝉ｽｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_DISTURBOBS			TRUE
/*--------------------------------------------------------------------------*/
/*		菴咲嶌陬懷─騾溷ｺｦ繧ｪ繝悶じ繝ｼ繝蝉ｽｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�							*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_EHVOBS_USE			TRUE
/*--------------------------------------------------------------------------*/
/*		菴咲ｽｮ謖�ｻ､遘ｻ蜍募ｹｳ蝮�ｽｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�								*/
/*--------------------------------------------------------------------------*/
#define CSW_SERVO_PCMD_FIL_USE		TRUE									  /* <S065> */
/*--------------------------------------------------------------------------*/
/*		菴咲ｽｮ謖�ｻ､遘ｻ蜍募ｹｳ蝮�謖�ｻ､譖ｴ譁ｰ蜻ｨ譛溯｣憺俣逕ｨ)菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�			*/
/*--------------------------------------------------------------------------*/
#define CSW_CMDRENEW_PCMD_FIL_USE	FALSE									  /* <S065> */
/*--------------------------------------------------------------------------*/
/*		菴咲ｽｮ謖�ｻ､High Filter 菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�			                        */
/*--------------------------------------------------------------------------*/
#define CSW_SERVO_PCMD_HIGH_FIL_USE   TRUE                                    /* <S0C3> */
/*--------------------------------------------------------------------------*/
/*		繝輔Ν繧ｯ繝ｭ繝ｼ繧ｺ繝牙宛蠕｡菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�								*/
/*--------------------------------------------------------------------------*/
#define CSW_FULC_NO_SUPPORT			0	/* 繝輔Ν繧ｯ蛻ｶ蠕｡辟｡蜉ｹ 					*/
#define CSW_FULC_BASIC_SUPPORT		1	/* 繝輔Ν繧ｯ蛻ｶ蠕｡譛牙柑					*/
#define CSW_FULC_SB_SUPPORT			2	/* 繝輔Ν繧ｯ蛻ｶ蠕｡譛牙柑(ScanB菴咲ｽｮ繝ｫ繝ｼ繝�	*/
//#define CSW_FULC_SA_SUPPORT		3	/* 繝輔Ν繧ｯ蛻ｶ蠕｡譛牙柑(ScanA菴咲ｽｮ繝ｫ繝ｼ繝�	*/
#define CSW_SVCF_FULC_MODE			CSW_FULC_BASIC_SUPPORT
/*--------------------------------------------------------------------------*/
/*		繝舌ャ繧ｯ繝ｩ繝�す菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_BACKLASHCOMP		TRUE		/* <S18B> */
/*--------------------------------------------------------------------------*/
/*		FB菴咲ｽｮ繝ｩ繝�メ菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_FBLATCH			TRUE									/* <S004> */
/*--------------------------------------------------------------------------*/
/*		讖溽ｨｮ螳夂ｾｩ�域ｩ溽ｨｮ縺ｫ繧医ｋ菴朱��ｯ�ｬ讖溯�縺ｮ譛臥┌��								*/
/*--------------------------------------------------------------------------*/
#define CSW_SVCF_CHKAMPLOL 			1										/* <S154> */
/*--------------------------------------------------------------------------*/
/*		蟇ｿ蜻ｽ莠域ｸｬ菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�										*/
/*--------------------------------------------------------------------------*/
//#define CSW_SVCF_LIFEPRED_USE 		TRUE									/* <S154> *//* <S186> */
#define CSW_SVCF_LIFEPRED_USE 		FALSE									/* <S154> *//* <S186> */
/*--------------------------------------------------------------------------*/
/*		豸郁ｲｻ髮ｻ蜉帙Δ繝九ち菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_POWER_MONITOR_USE		TRUE									/* <S020> */
/*--------------------------------------------------------------------------*/
/*		繝医Ξ繝ｼ繧ｹ繝阪ャ繝医Ρ繝ｼ繧ｯ繝医Μ繧ｬ菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�						*/
/*--------------------------------------------------------------------------*/
//#define CSW_NET_TRG_USE			FALSE									/* <S021> */	/* <S0C9> Delete */
#define CSW_NET_TRG_USE				TRUE									/* <S0C9> Delete */
/*--------------------------------------------------------------------------*/
/*		貂ｩ蠎ｦIC縺ｫ繧医ｋOH菴ｿ逕ｨ繝ｻ荳堺ｽｿ逕ｨ險ｭ螳�									*/
/*--------------------------------------------------------------------------*/
#define CSW_DETECT_OH_FROM_TEMPIC 	TRUE									  /* <S0ED> */
/*--------------------------------------------------------------------------*/
/*		  	  �ｽ-7/�ｽ-M Option												*/
/*--------------------------------------------------------------------------*/
#define PRODUCT_SERIES_SEVEN		FALSE									  /* <S09F> */
#if(PRODUCT_SERIES_SEVEN==FALSE)                                            /* <S09F> */
/*		HWBB蛻ｶ蠕｡螳溯｡碁∈謚�												 */
/*---------------------------------------------------------------------------*/
#define CSW_HW_HWBB_SUPPORT			FALSE                                   /* <S09F> */
/*--------------------------------------------------------------------------*/
/*		蛻�捉繝代Ν繧ｹ蜃ｺ蜉帛ｮ夂ｾｩ													*/
/*--------------------------------------------------------------------------*/
#define	SVFSEL_DIVOUT_USE			FALSE 	 /* 蛻�捉蜃ｺ蜉�TRUE:菴ｿ逕ｨ;FALSE:荳堺ｽｿ逕ｨ) <S09F> *//*<S02D>*/
/*--------------------------------------------------------------------------*/
/*		繧ｪ繝励す繝ｧ繝ｳ繧ｫ繝ｼ繝牙ｯｾ蠢懷ｮ夂ｾｩ											*/
/*--------------------------------------------------------------------------*/
/* CSW_OPT_CARD_OPTION_SUPPORT �撰ｽ假ｼ奇ｼ奇ｼ奇ｼ�								*/
/* 								   笏や狽笏や粕	繧ｻ繝ｼ繝輔ユ繧｣繝ｼ蟇ｾ蠢懃憾諷�		*/
/* 								   笏や狽笏�謖�ｻ､繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懃憾諷�		*/
/* 								   笏や粕 繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懃憾諷�*/
/* 								   笏�莠育ｴ�								*/
/*--------------------------------------------------------------------------*//* <S04D> */
/*	繧ｻ繝ｼ繝輔ユ繧｣繝ｼ蟇ｾ蠢懷ｮ夂ｾｩ													*/
#define CSW_SAFETY_NO_SUPPORT	0x0000			/* 繧ｻ繝ｼ繝輔ユ繧｣繝ｼ辟｡蜉ｹ 		*/
#define CSW_OPT_CARD_SAFETY		CSW_SAFETY_NO_SUPPORT

/*	謖�ｻ､繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懷ｮ夂ｾｩ													*/
#define CSW_CMDOPTION_NO_SUPPORT	0x0000		/* 謖�ｻ､繧ｪ繝励す繝ｧ繝ｳ辟｡蜉ｹ 		*/
#define CSW_OPT_CARD_CMDOPTION		CSW_CMDOPTION_NO_SUPPORT

/*	繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懷ｮ夂ｾｩ										*/
#define CSW_FBOPTION_NO_SUPPORT	0x0000		/* 繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ辟｡蜉ｹ */
#define CSW_FBOPTION_SUPPORT	0x0100		/* 繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ譛牙柑 */
#define CSW_OPT_CARD_FBOPTION		CSW_FBOPTION_NO_SUPPORT	   /* <S09F> */

/*	繧ｪ繝励す繝ｧ繝ｳ繧ｫ繝ｼ繝牙ｯｾ蠢懷ｮ夂ｾｩ												*/
#define CSW_OPT_CARD_NO_SUPPORT	0x0000		/* 繧ｪ繝励す繝ｧ繝ｳ繧ｫ繝ｼ繝臥┌蜉ｹ			*/
#define CSW_OPT_CARD			(CSW_OPT_CARD_SAFETY |\
								  CSW_OPT_CARD_CMDOPTION |\
								   CSW_OPT_CARD_FBOPTION )
/*--------------------------------------------------------------------------*/
/*		AD(繧ｪ繝ｼ繝舌ヲ繝ｼ繝�蛻ｶ蠕｡螳溯｡碁∈謚�								    */
/*--------------------------------------------------------------------------*/
//#define	SVFSEL_ONLY_OH_ADC_USE 		(0)		/* 1: ADC菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ		    */
#define	SVFSEL_ONLY_OH_ADC_USE 		(1)         /* <S19A>*/

/*	繝�す繝｡繝ｼ繧ｷ繝ｧ繝ｳ繝輔ぅ繝ｫ繧ｿ	<S202>											*/
#define CSW_DECIMATION	0x0001		/* CH1,2,3 Mode3 */

#else
/*		HWBB蛻ｶ蠕｡螳溯｡碁∈謚�												 */
/*---------------------------------------------------------------------------*/
#define CSW_HW_HWBB_SUPPORT			FALSE
/*--------------------------------------------------------------------------*/
/*		蛻�捉繝代Ν繧ｹ蜃ｺ蜉帛ｮ夂ｾｩ													*/
/*--------------------------------------------------------------------------*/
#define	SVFSEL_DIVOUT_USE			TRUE	 /* 蛻�捉蜃ｺ蜉�TRUE:菴ｿ逕ｨ;FALSE:荳堺ｽｿ逕ｨ)   *//*<S02D>*/
/*		繧ｪ繝励す繝ｧ繝ｳ繧ｫ繝ｼ繝牙ｯｾ蠢懷ｮ夂ｾｩ											*/
/*--------------------------------------------------------------------------*/
/* CSW_OPT_CARD_OPTION_SUPPORT �撰ｽ假ｼ奇ｼ奇ｼ奇ｼ�								*/
/* 								   笏や狽笏や粕	繧ｻ繝ｼ繝輔ユ繧｣繝ｼ蟇ｾ蠢懃憾諷�		*/
/* 								   笏や狽笏�謖�ｻ､繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懃憾諷�		*/
/* 								   笏や粕 繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懃憾諷�*/
/* 								   笏�莠育ｴ�								*/
/*--------------------------------------------------------------------------*//* <S04D> */
/*	繧ｻ繝ｼ繝輔ユ繧｣繝ｼ蟇ｾ蠢懷ｮ夂ｾｩ													*/
#define CSW_SAFETY_NO_SUPPORT	0x0000			/* 繧ｻ繝ｼ繝輔ユ繧｣繝ｼ辟｡蜉ｹ 		*/
#define CSW_OPT_CARD_SAFETY		CSW_SAFETY_NO_SUPPORT

/*	謖�ｻ､繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懷ｮ夂ｾｩ													*/
#define CSW_CMDOPTION_NO_SUPPORT	0x0000		/* 謖�ｻ､繧ｪ繝励す繝ｧ繝ｳ辟｡蜉ｹ 		*/
#define CSW_OPT_CARD_CMDOPTION		CSW_CMDOPTION_NO_SUPPORT

/*	繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ蟇ｾ蠢懷ｮ夂ｾｩ										*/
#define CSW_FBOPTION_NO_SUPPORT	0x0000		/* 繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ辟｡蜉ｹ */
#define CSW_FBOPTION_SUPPORT	0x0100		/* 繝輔ぅ繝ｼ繝峨ヰ繝�け繧ｪ繝励す繝ｧ繝ｳ譛牙柑 */
#define CSW_OPT_CARD_FBOPTION	CSW_FBOPTION_SUPPORT

/*	繧ｪ繝励す繝ｧ繝ｳ繧ｫ繝ｼ繝牙ｯｾ蠢懷ｮ夂ｾｩ												*/
#define CSW_OPT_CARD_NO_SUPPORT	0x0000		/* 繧ｪ繝励す繝ｧ繝ｳ繧ｫ繝ｼ繝臥┌蜉ｹ			*/
#define CSW_OPT_CARD			(CSW_OPT_CARD_SAFETY |\
								  CSW_OPT_CARD_CMDOPTION |\
								   CSW_OPT_CARD_FBOPTION )
/*--------------------------------------------------------------------------*/
/*		AD(繧ｪ繝ｼ繝舌ヲ繝ｼ繝�蛻ｶ蠕｡螳溯｡碁∈謚�								    */
/*--------------------------------------------------------------------------*/
#define	SVFSEL_ONLY_OH_ADC_USE 		(1)		/* 1: ADC菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ		    */

/*	繝�す繝｡繝ｼ繧ｷ繝ｧ繝ｳ繝輔ぅ繝ｫ繧ｿ	<S202>											*/
#define CSW_DECIMATION	0x0000		/* CH1,2 Mode3 CH3 Mode1	*/

#endif
/*--------------------------------------------------------------------------*/
/*		蝓ｺ譚ｿ貂ｩ蠎ｦ讀懷�讖溯�螳夂ｾｩ												*/
/*--------------------------------------------------------------------------*/
#define CSW_DETECT_TEMPERATURE		TRUE									/* <S03A> */
/*--------------------------------------------------------------------------*/
/*		蛻�捉繝代Ν繧ｹ蜃ｺ蜉帛ｮ夂ｾｩ													*/
/*--------------------------------------------------------------------------*/
//#define	SVFSEL_DIVOUT_USE			TRUE	 /* 蛻�捉蜃ｺ蜉�TRUE:菴ｿ逕ｨ;FALSE:荳堺ｽｿ逕ｨ) <S09F> *//*<S02D>*/

/****************************************************************************/
/*		繧ｹ繧ｭ繝｣繝ｳ蜻ｨ譛溘�螳溯｡後ち繧､繝溘Φ繧ｰ螳夂ｾｩ									*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/*		繧ｵ繧､繧ｯ繝ｫ繧ｿ繧､繝�ｮ夂ｾｩ													*/
/*--------------------------------------------------------------------------*/
#define NS_CHANGE_MS	1000000					/* [ms] 竍�[ns]螟画鋤菫よ焚		*/
#define US_CHANGE_MS	1000					/* [us] 竍�[ms]螟画鋤菫よ焚		*/
#define NS_CHANGE_US	1000					/* [ns] 竍�[us]螟画鋤菫よ焚		*/

/*--------------------------------------------------------------------------*/
/*		JL-077ﾎｼ繧ｹ繧ｭ繝｣繝ｳ蜻ｨ譛�											*/
/*--------------------------------------------------------------------------*/
#define KPI_MACYCLENS	15625							/* ScanI Cycle[ns]	*/
#define KPI_MBCYCLENS	62500							/* MprgB Cycle[ns]	*/
#define KPI_SACYCLENS	62500							/* ScanA Cycle[ns]	*/
#define KPI_SACYCLEUS	((KPI_SACYCLENS + 500)/1000)	/* ScanA Cycle[us]	*/
#define KPI_MBCYCLEUS	((KPI_MBCYCLENS + 500)/1000)	/* MprgB Cycle[us]	*/
#define KPI_SACOUNTMS	(1000000/KPI_SACYCLENS)			/* ScanA Cycle Count per 1ms */

/*--------------------------------------------------------------------------*/
/*		ScanB蜻ｨ譛�ScanB蜻ｨ譛溘�繝代Λ繝｡繝ｼ繧ｿ縺ｧ豎ｺ縺ｾ繧九◆繧√�繝�ヵ繧ｩ繝ｫ繝亥�繧貞ｮ夂ｾｩ)	*/
/*--------------------------------------------------------------------------*/
#define KPI_SBCYCLE_BASE_NS		125000		/* ScanB Base Cycle Time [ns]	*/
#define KPI_SBCYCLEUS			(KPI_SBCYCLE_BASE_NS / 1000 )	  /* [us]	*//* <S039> */
#define KPI_SBCYCLE_TIME		2			/* ScanB Cycle Time [125us]		*/
#define KPI_SBCOUNTMS			(1000000/KPI_SBCYCLE_BASE_NS)	/* ScanB Cycle Count per 1 ms */
#define	KPI_SCANB_MS( ms )		( (ms)*KPI_SBCOUNTMS )

/*--------------------------------------------------------------------------*/
/*		ScanC蜻ｨ譛�													 	*/
/*--------------------------------------------------------------------------*/
#define KPI_SCCYCLEMS			2
#define KPI_SCCYCLEUS			( KPI_SCCYCLEMS * 1000  )
#define KPI_SCANC_MS( ms )		( (ms)/KPI_SCCYCLEMS )

/*--------------------------------------------------------------------------*/
/*		MECHATROLINK蜃ｦ逅�ち繧､繝溘Φ繧ｰ螳夂ｾｩ										*/
/*--------------------------------------------------------------------------*/
#define SCAN_N_DEFCYCLE			1				/* 繝�ヵ繧ｩ繝ｫ繝磯�菫｡TASK蜻ｨ譛歇ScanB繧ｫ繧ｦ繝ｳ繝� */
#define CFG_TIO_BASE_TIME		500				/* 繧ｵ繝ｼ繝懷宛蠕｡蜻ｨ譛歇0.25us]	*/
//#define CFG_POSERR_DELAY		KPI_MBCYCLENS	/* 菴咲ｽｮ蛛丞ｷｮ繝｢繝九ち逕ｨ陬懈ｭ｣譎る俣[ns] */
#define CFG_POSERR_DELAY		KPI_SBCYCLE_BASE_NS	/* 菴咲ｽｮ蛛丞ｷｮ繝｢繝九ち逕ｨ陬懈ｭ｣譎る俣[ns] *//* <S0C0> */



/*****************************************************************************/
/*		髮ｻ貅宣未騾｣縲？W蛻ｶ蠕｡讖溯�SW螳夂ｾｩ											 */
/*****************************************************************************/
/*---------------------------------------------------------------------------*/
/*		DMA霆｢騾∵怏蜉ｹ繧ｹ繧､繝�メ													 */
/*---------------------------------------------------------------------------*/
#define CSW_MAICRO_DMA_TRANSFER	FALSE	/* Enable DMA transfer for JL077-uP and CPU */
#define CSW_M3_DMA_TRANSFER		FALSE	/* Enable DMA transfer for JL100 and CPU */

/*---------------------------------------------------------------------------*/
/*		PN Voltage Detect by Decimation Filter(PnE0C.bit13, PnE7C縺ｧ螳夂ｾｩ)	 */
/*---------------------------------------------------------------------------*/
/* PnE0C.bit13=1 & 縺薙�SW縺ｧ蛻�ｊ譖ｿ縺�蠕後〒繝代Λ繝｡繝ｼ繧ｿOnly縺ｫ縺吶ｋ) */
#define AD_DETECT_DECIMATION		TRUE

#define CSW_HW_POWER_MANAGE			TRUE	/* 髮ｻ貅千ｮ｡逅��逅�ｮ溯｡碁∈謚�*/
#define CSW_HW_REGENERATION_CTRL	TRUE	/* 蝗樒函蜃ｦ逅�∈謚�/
#define CSW_HW_CHKRUSH				TRUE	/* 遯∝�謚ｵ謚鈴℃雋�差讀懷�驕ｸ謚�*/
#define CSW_HW_DC_DETECT			TRUE	/* DC髮ｻ貅宣崕蝨ｧ讀懷�驕ｸ謚�*/
#define CSW_HW_INTERNAL_FAN			TRUE	/* 蜀�鳩繝輔ぃ繝ｳ縺ｮ譛臥┌ */
#define CSW_HW_DCBUS				FALSE	/* DC蜈･蜉帙Δ繝ｼ繝蛾∈謚�*/


/*---------------------------------------------------------------------------*/
/*		DB蛻ｶ蠕｡螳溯｡碁∈謚�													 */
/*---------------------------------------------------------------------------*/
#define CSW_HW_DB_CTRL				TRUE
/*---------------------------------------------------------------------------*/
/*		HWBB蛻ｶ蠕｡螳溯｡碁∈謚�												 */
/*---------------------------------------------------------------------------*/
//#define CSW_HW_HWBB_SUPPORT			FALSE  /* <S09F> */

/*---------------------------------------------------------------------------*/
/*		隍�焚霆ｸ蜷梧凾繧､繝ｳ繧ｿ繝ｭ繝�け讖溯�驕ｸ謚�									 */
/*---------------------------------------------------------------------------*/
#ifdef CSW_BSP_SGDZ_IFBS80
#define CSW_SVON_INTERLOCK
#define SVON_LOCK_GRP1				0x00000006		// Axis 1 and 2 on CB2 board
#define SVON_LOCK_GRP2				0x000000f8		// Axis 3 and 8 on CC board
#define SVON_LOCK_GRP1_MAX			1
#define SVON_LOCK_GRP2_MAX			3

#define OH_ALM_SLOT_GRP1			0x00000006		// Axis 1 and 2 on CB2 board
#define OH_ALM_SLOT_GRP2			0x000000f8		// Axis 3 and 8 on CC board

#endif	//CSW_BSP_SGDZ_IFBS80

/*****************************************************************************/
/*		縺昴�莉�															 */
/*****************************************************************************/
/*---------------------------------------------------------------------------*/
/*		Amp and Mot Type Definition											 */
/*---------------------------------------------------------------------------*/
#define MOTTYPE_ROTARY				0x00		/* 蝗櫁ｻ｢蝙九Δ繝ｼ繧ｿ繝ｼ */
#define MOTTYPE_LINEAR				0x01		/* 繝ｪ繝九い繝｢繝ｼ繧ｿ繝ｼ */
/*---------------------------------------------------------------------------*/
/*		Un Monitor Average time												 */
/*---------------------------------------------------------------------------*/
#define SVCDEF_UNSPD_SCMEANNUM		((64 + KPI_SCCYCLEMS - 1) / KPI_SCCYCLEMS)
#define SVCDEF_UNSPD_SCMEANTMMS		( KPI_SCCYCLEMS * SVCDEF_UNSPD_SCMEANNUM )
/*---------------------------------------------------------------------------*/
/*		Motor Selection Definition											 */
/*---------------------------------------------------------------------------*//* <S04D> */
#define	MOTSEL_YASKAWA		0x00				/* 螳牙ｷ昴Δ繝ｼ繧ｿ(讓呎ｺ�			 */
#define	MOTSEL_NONYASKAWA	0xF0				/* 莉也､ｾ繝｢繝ｼ繧ｿ				 */
/*---------------------------------------------------------------------------*/
/*		Encoder Selection Definition										 */
/*---------------------------------------------------------------------------*//* <S04D> */
#define	ENCSEL_SERIAL		0x00				/* 繧ｷ繝ｪ繧｢繝ｫ繧ｨ繝ｳ繧ｳ繝ｼ繝�	 */
#define	ENCSEL_PULSE		0x10				/* �｡�｢繝代Ν繧ｨ繝ｳ繧ｳ繝ｼ繝�	 */
#define	ENCSEL_ANALOG 		0x20				/* 繧｢繝翫Ο繧ｰ繧ｨ繝ｳ繧ｳ繝ｼ繝�	 */
#define	ENCSEL_ENCLESS		0xFF				/* 繧ｨ繝ｳ繧ｳ繝ｼ繝�Ξ繧ｹ			 */


/*---------------------------------------------------------------------------*/
/*		Un Monitor Average time												 */
/*---------------------------------------------------------------------------*/
#define SVCDEF_UNSPD_SCMEANNUM		((64 + KPI_SCCYCLEMS - 1) / KPI_SCCYCLEMS)
#define SVCDEF_UNSPD_SCMEANTMMS		( KPI_SCCYCLEMS * SVCDEF_UNSPD_SCMEANNUM )

/*---------------------------------------------------------------------------*/
/*		Normalize Multiplier Factor											 */
/*---------------------------------------------------------------------------*/
#define NORM_TRQ_TO_IQ			3840000		/* TrqRef(2^24) --> IqRef(15000) */
#define NORM_IQ_TO_TRQ			73300775	/* IqRef(15000) --> TrqRef(2^24) */

/*---------------------------------------------------------------------------*/
/*		<S031> LCD Operator Parameter Copy									 */
/*---------------------------------------------------------------------------*/
#define	SVFSEL_LCDPRMCOPY_USE 	(1)		/* 1: LCD繝代Λ繝｡繝ｼ繧ｿ繧ｳ繝斐�菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ			*/

extern LONG NumberOfAxes;		// 蛻ｶ蠕｡霆ｸ謨ｰ



#endif /* _SYSTEM_SETTING_H */

/* <S056>	Start */
/*---------------------------------------------------------------------------*/
/*		Information Register Definition										 */
/*---------------------------------------------------------------------------*/
#define SERVO_ID			0x071	/* 繧ｵ繝ｼ繝懈ｩ溽ｨｮID : SFD7S_M2 = 0x0071 */
/* <S173> Start */
#define INPUT_SIGNAL_INFO	0x0000007f	/* 蜈･蜉帑ｿ｡蜿ｷ 譛牙柑/辟｡蜉ｹ諠�� */
										/* SI0?SI6:譛牙柑 */
										/* SI7?SI31:辟｡蜉ｹ */

/* 蜃ｺ蜉帑ｿ｡蜿ｷ 譛牙柑/辟｡蜉ｹ諠��隕狗峩縺励�轤ｺ縲√さ繝｡繝ｳ繝医い繧ｦ繝�*/
#if 0
#define OUTPUT_SIGNAL_INFO	0x0000000E	/* 蜃ｺ蜉帑ｿ｡蜿ｷ 譛牙柑/辟｡蜉ｹ諠�� */
										/* SO1?SO3:譛牙柑 */
										/* SO0,SO4?SO31:辟｡蜉ｹ */
#endif

/* 蜃ｺ蜉帑ｿ｡蜿ｷ 譛牙柑/辟｡蜉ｹ諠��隕狗峩縺�*/
#define OUTPUT_SIGNAL_INFO	0x0000000f	/* 蜃ｺ蜉帑ｿ｡蜿ｷ 譛牙柑/辟｡蜉ｹ諠�� */
										/* ALM,SO1縲彜O3:譛牙柑 */
										/* SO4縲彜O31:辟｡蜉ｹ */
/* <S173> End */
/* <S056>	End */

#define ALM_WARNING_AUTO_CLEAR	FALSE	/* <S184> 繝ｯ繝ｼ繝九Φ繧ｰ閾ｪ蜍輔け繝ｪ繧｢驕ｸ謚�*/


/*---------------------------------------------------------------------------*/
/*		<S09B> Auto Gain Change												 */
/*---------------------------------------------------------------------------*/
#define		SVFSEL_ATGAINCHANGE_USE	(1)				/* 1: 閾ｪ蜍輔ご繧､繝ｳ蛻�崛菴ｿ逕ｨ 0: 譛ｪ菴ｿ逕ｨ				*/
