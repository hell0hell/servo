/****************************************************************************
Description	: Definitions for each product functionality
----------------------------------------------------------------------------
File name		: SpecCommon.h		<S008>

Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

----------------------------------------------------------------------------
Changes :
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2013.08.17	created

******************************************************************************/
#ifndef _SPEC_COMMON_H
#define _SPEC_COMMON_H



/****************************************************************************
	Define the FirmWare Version
****************************************************************************/
#if (FLOAT_USE==TRUE)	/* Float対応版は暫定的にマイナーverの先頭にもFを付ける		<S00A>	*/
#define SERVO_FIRM_VER		0xF009F016	/* ソフトウェアVer. */
										/* byte3,4:メジャーVer. */
										/* byte1,2:マイナーVer. */
#else
#define SERVO_FIRM_VER		0xF0090016	/* ソフトウェアVer. */
										/* byte3,4:メジャーVer. */
										/* byte1,2:マイナーVer. */
#endif

/* <S056>	Start */
/********************************************************************/
/*																	*/
/*	「SERVO_YSPEC_VER」にてY仕、FT仕様、EX仕様のVerを管理するため、	*/
/*	定義名称を「SERVO_SPECIAL_SPEC_VER」へ変更します。				*/
/*																	*/
/********************************************************************/
//#define SERVO_YSPEC_VER		0x0000	/* B4-B15:Y仕様Ver. */
										/* B0-B3:Y仕様Ver.改版番号 */

#define SERVO_SPECIAL_SPEC_VER	0x0000	/* B4-B15:特殊Ver. */
										/* B0-B3:特殊Ver.改版番号 */


#define REG_SPEC_VER	0x0E000101	/* レジスタスペックバージョン */
									/* B0-B7:デバイスID */
									/* B8-B15:サプライヤーID */
									/* B16-B23:マイナーバージョン */
									/* B24-B31:メジャーバージョン */

#define SERVO_BOOT_VER	0x0002		/* ソフトウェアブートVer. */

/************************************************************************************************************/
/*																											*/
/*パラメータVer.管理方法は以下とする																		*/
/*	・出荷時設定を変更した場合はデータ改版番号を＋１する。(データ改版番号:PRM_DATA_VERSION)					*/
/*	・パラメータのフォーマット変更時（パラメータ数の増減、配置変更）はフォーマット改版番号を＋１し、		*/
/*	　かつデータ改版番号を＋１する。(フォーマット改版番号:PRM_FORMAT_VERSION)								*/
/*	・改版番号は１６進数（バイナリ）で定義する。															*/
/*																											*/
/************************************************************************************************************/
/* ★レジスタ対応で、レジスタ仕様書(900-133-284)の管理方法に合わせる為、「PRM_VERSION」でのパラメータVer.管理を辞め、以下の2つでVer.管理をするように変更しました。 */
//#define PRM_VERSION			0x0001		/* パラメータVer. */
#define PRM_DATA_VERSION		0x0001		/* パラメータVer.(データ改版番号) */
#define PRM_FORMAT_VERSION		0x0001		/* パラメータVer.(フォーマット改版番号) */
/* <S056>	End */



/****************************************************************************
	Define the CSW_NETIF_TYPE
****************************************************************************/
#define	NETIF_ANLGPLS_TYPE   1			/* Analog/Pulse type				*/
#define	NETIF_M2_TYPE        2			/* MECHATROLINK-II type				*/
#define	NETIF_M3_TYPE        3			/* MECHATROLINK-III type			*/
#define	NETIF_CM_TYPE   	 4			/* ECAT一体とCMDは同じ扱いにする(可能な限り) <S008>	*/
#define	NETIF_SYNQNET_TYPE   5			/* Synqnet??						*/
#define	NETIF_ECAT_TYPE   	 6			/* EtherCAT処理をSoCで行う場合		*/



/****************************************************************************
	Define the CSW_BOARD_TYPE	<S0A2>
****************************************************************************/
#define	SGD7S_TYPE			1			/* Power Board is SGD7S Type		*/
#define	SGD7W_TYPE			2			/* Power Board is SGD7W Type		*/

#endif /* _SPEC_COMMON_H */
/***************************************** end of file **********************************************/
