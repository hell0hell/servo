/************************************************************************************************************/
/*																											*/
/*																											*/
/*		ARMPF_VIC.h : MercurySoC ペリフェラル VICヘッダ														*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		割り込みコントローラ(VIC)関連ヘッダファイル															*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.14  Y.Tanaka		初版															*/
/*																											*/
/************************************************************************************************************/
#include	"ARMPF.h"


#ifndef	_ARMPF_VIC_H_
#define	_ARMPF_VIC_H_


#define	ARMPF_VIC_BASE	0xEFFDF000							/* VICベースアドレス							*/

/*----------------------------------------------------------------------------------------------------------*/
/*		割り込み実行部選択																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_IRQ		0x00000000						/* IRQ割り込み									*/
#define	ARMPF_VIC_FIQ		0x00000001						/* FIQ割り込み									*/

/*----------------------------------------------------------------------------------------------------------*/
/*		割り込み検出方法選択	PLS																			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_LVL		0x00							/* レベル検出									*/
#define	ARMPF_VIC_EDGE		0x01							/* エッジ検出									*/

/*----------------------------------------------------------------------------------------------------------*/
/*		割り込み検出有効レベル選択	LVLC0-7																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_HIGH		0x01							/* ハイレベルで検出								*/
#define	ARMPF_VIC_LOW 		0x02							/* ローレベルで検出								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		割り込み検出有効エッジ選択	EDGC0-7																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_RISE		0x01							/* 立ち上がりで検出								*/
#define	ARMPF_VIC_FALL		0x02							/* 立下りで検出									*/
#define	ARMPF_VIC_BIEDGE	0x03							/* 両側エッジで検出								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		割り込み優先度	PRL0-127																			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_PRIORYTY0		0							/* 優先度0（MAX）								*/
#define	ARMPF_VIC_PRIORYTY1		1							/* 優先度1										*/
#define	ARMPF_VIC_PRIORYTY2		2							/* 優先度2										*/
#define	ARMPF_VIC_PRIORYTY3		3							/* 優先度3										*/
#define	ARMPF_VIC_PRIORYTY4		4							/* 優先度4										*/
#define	ARMPF_VIC_PRIORYTY5		5							/* 優先度5										*/
#define	ARMPF_VIC_PRIORYTY6		6							/* 優先度6										*/
#define	ARMPF_VIC_PRIORYTY7		7							/* 優先度7										*/
#define	ARMPF_VIC_PRIORYTY8		8							/* 優先度8										*/
#define	ARMPF_VIC_PRIORYTY9		9							/* 優先度9										*/
#define	ARMPF_VIC_PRIORYTY10	10							/* 優先度10										*/
#define	ARMPF_VIC_PRIORYTY11	11							/* 優先度11										*/
#define	ARMPF_VIC_PRIORYTY12	12							/* 優先度12										*/
#define	ARMPF_VIC_PRIORYTY13	13							/* 優先度13										*/
#define	ARMPF_VIC_PRIORYTY14	14							/* 優先度14										*/
#define	ARMPF_VIC_PRIORYTY15	15							/* 優先度15（MIN）								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		VIC register																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_IRQS0		(ARMPF_VIC_BASE + 0x000)		/* IRQステータス・レジスタ0						*/
#define	ARMPF_VIC_IRQS1		(ARMPF_VIC_BASE + 0x004)		/* IRQステータス・レジスタ1						*/
//#define	ARMPF_VIC_IRQS2		(ARMPF_VIC_BASE + 0x008)		/* IRQステータス・レジスタ2						*//* JL-086使用不可 */
//#define	ARMPF_VIC_IRQS3		(ARMPF_VIC_BASE + 0x00C)		/* IRQステータス・レジスタ3						*//* JL-086使用不可 */
#define	ARMPF_VIC_FIQS0		(ARMPF_VIC_BASE + 0x020)		/* FIQステータス・レジスタ0						*/
#define	ARMPF_VIC_FIQS1		(ARMPF_VIC_BASE + 0x024)		/* FIQステータス・レジスタ1						*/
//#define	ARMPF_VIC_FIQS2		(ARMPF_VIC_BASE + 0x028)		/* FIQステータス・レジスタ2						*//* JL-086使用不可 */
//#define	ARMPF_VIC_FIQS3		(ARMPF_VIC_BASE + 0x02C)		/* FIQステータス・レジスタ3						*//* JL-086使用不可 */
#define	ARMPF_VIC_RAIS0		(ARMPF_VIC_BASE + 0x040)		/* 割り込み入力ステータス・レジスタ0			*/
#define	ARMPF_VIC_RAIS1		(ARMPF_VIC_BASE + 0x044)		/* 割り込み入力ステータス・レジスタ1			*/
//#define	ARMPF_VIC_RAIS2		(ARMPF_VIC_BASE + 0x048)		/* 割り込み入力ステータス・レジスタ2			*//* JL-086使用不可 */
//#define	ARMPF_VIC_RAIS3		(ARMPF_VIC_BASE + 0x04C)		/* 割り込み入力ステータス・レジスタ3			*//* JL-086使用不可 */
#define	ARMPF_VIC_ISL0		(ARMPF_VIC_BASE + 0x060)		/* IRQ/FIQ割り込み選択レジスタ0					*/
#define	ARMPF_VIC_ISL1		(ARMPF_VIC_BASE + 0x064)		/* IRQ/FIQ割り込み選択レジスタ1					*/
//#define	ARMPF_VIC_ISL2		(ARMPF_VIC_BASE + 0x068)		/* IRQ/FIQ割り込み選択レジスタ2					*//* JL-086使用不可 */
//#define	ARMPF_VIC_ISL3		(ARMPF_VIC_BASE + 0x06C)		/* IRQ/FIQ割り込み選択レジスタ3					*//* JL-086使用不可 */
#define	ARMPF_VIC_IEN0		(ARMPF_VIC_BASE + 0x080)		/* 割り込みイネーブル・レジスタ0				*/
#define	ARMPF_VIC_IEN1		(ARMPF_VIC_BASE + 0x084)		/* 割り込みイネーブル・レジスタ1				*/
//#define	ARMPF_VIC_IEN2		(ARMPF_VIC_BASE + 0x088)		/* 割り込みイネーブル・レジスタ2				*//* JL-086使用不可 */
//#define	ARMPF_VIC_IEN3		(ARMPF_VIC_BASE + 0x08C)		/* 割り込みイネーブル・レジスタ3				*//* JL-086使用不可 */
#define	ARMPF_VIC_IEC0		(ARMPF_VIC_BASE + 0x0A0)		/* 割り込みイネーブル・クリア・レジスタ0		*/
#define	ARMPF_VIC_IEC1		(ARMPF_VIC_BASE + 0x0A4)		/* 割り込みイネーブル・クリア・レジスタ1		*/
//#define	ARMPF_VIC_IEC2		(ARMPF_VIC_BASE + 0x0A8)		/* 割り込みイネーブル・クリア・レジスタ2		*//* JL-086使用不可 */
//#define	ARMPF_VIC_IEC3		(ARMPF_VIC_BASE + 0x0AC)		/* 割り込みイネーブル・クリア・レジスタ3		*//* JL-086使用不可 */
#define	ARMPF_VIC_SWI0		(ARMPF_VIC_BASE + 0x0C0)		/* ソフトウェア割り込みレジスタ0				*/
#define	ARMPF_VIC_SWI1		(ARMPF_VIC_BASE + 0x0C4)		/* ソフトウェア割り込みレジスタ1				*/
//#define	ARMPF_VIC_SWI2		(ARMPF_VIC_BASE + 0x0C8)		/* ソフトウェア割り込みレジスタ2				*//* JL-086使用不可 */
//#define	ARMPF_VIC_SWI3		(ARMPF_VIC_BASE + 0x0CC)		/* ソフトウェア割り込みレジスタ3				*//* JL-086使用不可 */
#define	ARMPF_VIC_SWC0		(ARMPF_VIC_BASE + 0x0E0)		/* ソフトウェア割り込みクリア・レジスタ0		*/
#define	ARMPF_VIC_SWC1		(ARMPF_VIC_BASE + 0x0E4)		/* ソフトウェア割り込みクリア・レジスタ1		*/
//#define	ARMPF_VIC_SWC2		(ARMPF_VIC_BASE + 0x0E8)		/* ソフトウェア割り込みクリア・レジスタ2		*//* JL-086使用不可 */
//#define	ARMPF_VIC_SWC3		(ARMPF_VIC_BASE + 0x0EC)		/* ソフトウェア割り込みクリア・レジスタ3		*//* JL-086使用不可 */
#define	ARMPF_VIC_PLS0		(ARMPF_VIC_BASE + 0x100)		/* 割り込み検出タイプ選択レジスタ0				*/
#define	ARMPF_VIC_PLS1		(ARMPF_VIC_BASE + 0x104)		/* 割り込み検出タイプ選択レジスタ1				*/
//#define	ARMPF_VIC_PLS2		(ARMPF_VIC_BASE + 0x108)		/* 割り込み検出タイプ選択レジスタ2				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PLS3		(ARMPF_VIC_BASE + 0x10C)		/* 割り込み検出タイプ選択レジスタ3				*//* JL-086使用不可 */
#define	ARMPF_VIC_PIC0		(ARMPF_VIC_BASE + 0x120)		/* エッジ検出ビット・クリア・レジスタ0			*/
#define	ARMPF_VIC_PIC1		(ARMPF_VIC_BASE + 0x124)		/* エッジ検出ビット・クリア・レジスタ1			*/
//#define	ARMPF_VIC_PIC2		(ARMPF_VIC_BASE + 0x128)		/* エッジ検出ビット・クリア・レジスタ2			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PIC3		(ARMPF_VIC_BASE + 0x12C)		/* エッジ検出ビット・クリア・レジスタ3			*//* JL-086使用不可 */
#define	ARMPF_VIC_EDGC0		(ARMPF_VIC_BASE + 0x140)		/* 割り込みエッジ・コントロール・レジスタ0		*/
#define	ARMPF_VIC_EDGC1		(ARMPF_VIC_BASE + 0x144)		/* 割り込みエッジ・コントロール・レジスタ1		*/
#define	ARMPF_VIC_EDGC2		(ARMPF_VIC_BASE + 0x148)		/* 割り込みエッジ・コントロール・レジスタ2		*/
#define	ARMPF_VIC_EDGC3		(ARMPF_VIC_BASE + 0x14C)		/* 割り込みエッジ・コントロール・レジスタ3		*/
//#define	ARMPF_VIC_EDGC4		(ARMPF_VIC_BASE + 0x150)		/* 割り込みエッジ・コントロール・レジスタ4		*//* JL-086使用不可 */
//#define	ARMPF_VIC_EDGC5		(ARMPF_VIC_BASE + 0x154)		/* 割り込みエッジ・コントロール・レジスタ5		*//* JL-086使用不可 */
//#define	ARMPF_VIC_EDGC6		(ARMPF_VIC_BASE + 0x158)		/* 割り込みエッジ・コントロール・レジスタ6		*//* JL-086使用不可 */
//#define	ARMPF_VIC_EDGC7		(ARMPF_VIC_BASE + 0x15C)		/* 割り込みエッジ・コントロール・レジスタ7		*//* JL-086使用不可 */
#define	ARMPF_VIC_LVLC0		(ARMPF_VIC_BASE + 0x180)		/* 割り込みレベル・コントロール・レジスタ0		*/
#define	ARMPF_VIC_LVLC1		(ARMPF_VIC_BASE + 0x184)		/* 割り込みレベル・コントロール・レジスタ1		*/
#define	ARMPF_VIC_LVLC2		(ARMPF_VIC_BASE + 0x188)		/* 割り込みレベル・コントロール・レジスタ2		*/
#define	ARMPF_VIC_LVLC3		(ARMPF_VIC_BASE + 0x18C)		/* 割り込みレベル・コントロール・レジスタ3		*/
//#define	ARMPF_VIC_LVLC4		(ARMPF_VIC_BASE + 0x190)		/* 割り込みレベル・コントロール・レジスタ4		*//* JL-086使用不可 */
//#define	ARMPF_VIC_LVLC5		(ARMPF_VIC_BASE + 0x194)		/* 割り込みレベル・コントロール・レジスタ5		*//* JL-086使用不可 */
//#define	ARMPF_VIC_LVLC6		(ARMPF_VIC_BASE + 0x198)		/* 割り込みレベル・コントロール・レジスタ6		*//* JL-086使用不可 */
//#define	ARMPF_VIC_LVLC7		(ARMPF_VIC_BASE + 0x19C)		/* 割り込みレベル・コントロール・レジスタ7		*//* JL-086使用不可 */
#define	ARMPF_VIC_PRLM		(ARMPF_VIC_BASE + 0x1C0)		/* 割り込み優先レベル・マスク・レジスタ			*/
#define	ARMPF_VIC_PRLC		(ARMPF_VIC_BASE + 0x1C4)		/* 割り込み優先レベル・マスク・クリア・レジスタ	*/
#define	ARMPF_VIC_UEN 		(ARMPF_VIC_BASE + 0x1C8)		/* ユーザーモード・イネーブル・レジスタ			*/
#define	ARMPF_VIC_HVA 		(ARMPF_VIC_BASE + 0x200)		/* 割り込みアドレス・レジスタ					*/
#define	ARMPF_VIC_ISS0 		(ARMPF_VIC_BASE + 0x210)		/* 割り込みサービス・ステータス・レジスタ0		*/
#define	ARMPF_VIC_ISS1 		(ARMPF_VIC_BASE + 0x214)		/* 割り込みサービス・ステータス・レジスタ1		*/
//#define	ARMPF_VIC_ISS2 		(ARMPF_VIC_BASE + 0x218)		/* 割り込みサービス・ステータス・レジスタ2		*//* JL-086使用不可 */
//#define	ARMPF_VIC_ISS3 		(ARMPF_VIC_BASE + 0x21C)		/* 割り込みサービス・ステータス・レジスタ3		*//* JL-086使用不可 */
#define	ARMPF_VIC_ISC0 		(ARMPF_VIC_BASE + 0x230)		/* 割り込みサービス・カレント・レジスタ0		*/
#define	ARMPF_VIC_ISC1 		(ARMPF_VIC_BASE + 0x234)		/* 割り込みサービス・カレント・レジスタ1		*/
//#define	ARMPF_VIC_ISC2 		(ARMPF_VIC_BASE + 0x238)		/* 割り込みサービス・カレント・レジスタ2		*//* JL-086使用不可 */
//#define	ARMPF_VIC_ISC3 		(ARMPF_VIC_BASE + 0x23C)		/* 割り込みサービス・カレント・レジスタ3		*//* JL-086使用不可 */
#define	ARMPF_VIC_VAD0		(ARMPF_VIC_BASE + 0x400)		/* 割り込みアドレス格納レジスタ0				*/
#define	ARMPF_VIC_VAD1		(ARMPF_VIC_BASE + 0x404)		/* 割り込みアドレス格納レジスタ1				*/
#define	ARMPF_VIC_VAD2		(ARMPF_VIC_BASE + 0x408)		/* 割り込みアドレス格納レジスタ2				*/
#define	ARMPF_VIC_VAD3		(ARMPF_VIC_BASE + 0x40C)		/* 割り込みアドレス格納レジスタ3				*/
#define	ARMPF_VIC_VAD4		(ARMPF_VIC_BASE + 0x410)		/* 割り込みアドレス格納レジスタ4				*/
#define	ARMPF_VIC_VAD5		(ARMPF_VIC_BASE + 0x414)		/* 割り込みアドレス格納レジスタ5				*/
#define	ARMPF_VIC_VAD6		(ARMPF_VIC_BASE + 0x418)		/* 割り込みアドレス格納レジスタ6				*/
#define	ARMPF_VIC_VAD7		(ARMPF_VIC_BASE + 0x41C)		/* 割り込みアドレス格納レジスタ7				*/
#define	ARMPF_VIC_VAD8		(ARMPF_VIC_BASE + 0x420)		/* 割り込みアドレス格納レジスタ8				*/
#define	ARMPF_VIC_VAD9		(ARMPF_VIC_BASE + 0x424)		/* 割り込みアドレス格納レジスタ9				*/
#define	ARMPF_VIC_VAD10		(ARMPF_VIC_BASE + 0x428)		/* 割り込みアドレス格納レジスタ10				*/
#define	ARMPF_VIC_VAD11		(ARMPF_VIC_BASE + 0x42C)		/* 割り込みアドレス格納レジスタ11				*/
#define	ARMPF_VIC_VAD12		(ARMPF_VIC_BASE + 0x430)		/* 割り込みアドレス格納レジスタ12				*/
#define	ARMPF_VIC_VAD13		(ARMPF_VIC_BASE + 0x434)		/* 割り込みアドレス格納レジスタ13				*/
#define	ARMPF_VIC_VAD14		(ARMPF_VIC_BASE + 0x438)		/* 割り込みアドレス格納レジスタ14				*/
#define	ARMPF_VIC_VAD15		(ARMPF_VIC_BASE + 0x43C)		/* 割り込みアドレス格納レジスタ15				*/
#define	ARMPF_VIC_VAD16		(ARMPF_VIC_BASE + 0x440)		/* 割り込みアドレス格納レジスタ16				*/
#define	ARMPF_VIC_VAD17		(ARMPF_VIC_BASE + 0x444)		/* 割り込みアドレス格納レジスタ17				*/
#define	ARMPF_VIC_VAD18		(ARMPF_VIC_BASE + 0x448)		/* 割り込みアドレス格納レジスタ18				*/
#define	ARMPF_VIC_VAD19		(ARMPF_VIC_BASE + 0x44C)		/* 割り込みアドレス格納レジスタ19				*/
#define	ARMPF_VIC_VAD20		(ARMPF_VIC_BASE + 0x450)		/* 割り込みアドレス格納レジスタ20				*/
#define	ARMPF_VIC_VAD21		(ARMPF_VIC_BASE + 0x454)		/* 割り込みアドレス格納レジスタ21				*/
#define	ARMPF_VIC_VAD22		(ARMPF_VIC_BASE + 0x458)		/* 割り込みアドレス格納レジスタ22				*/
#define	ARMPF_VIC_VAD23		(ARMPF_VIC_BASE + 0x45C)		/* 割り込みアドレス格納レジスタ23				*/
#define	ARMPF_VIC_VAD24		(ARMPF_VIC_BASE + 0x460)		/* 割り込みアドレス格納レジスタ24				*/
#define	ARMPF_VIC_VAD25		(ARMPF_VIC_BASE + 0x464)		/* 割り込みアドレス格納レジスタ25				*/
#define	ARMPF_VIC_VAD26		(ARMPF_VIC_BASE + 0x468)		/* 割り込みアドレス格納レジスタ26				*/
#define	ARMPF_VIC_VAD27		(ARMPF_VIC_BASE + 0x46C)		/* 割り込みアドレス格納レジスタ27				*/
#define	ARMPF_VIC_VAD28		(ARMPF_VIC_BASE + 0x470)		/* 割り込みアドレス格納レジスタ28				*/
#define	ARMPF_VIC_VAD29		(ARMPF_VIC_BASE + 0x474)		/* 割り込みアドレス格納レジスタ29				*/
#define	ARMPF_VIC_VAD30		(ARMPF_VIC_BASE + 0x478)		/* 割り込みアドレス格納レジスタ30				*/
#define	ARMPF_VIC_VAD31		(ARMPF_VIC_BASE + 0x47C)		/* 割り込みアドレス格納レジスタ31				*/
#define	ARMPF_VIC_VAD32		(ARMPF_VIC_BASE + 0x480)		/* 割り込みアドレス格納レジスタ32				*/
#define	ARMPF_VIC_VAD33		(ARMPF_VIC_BASE + 0x484)		/* 割り込みアドレス格納レジスタ33				*/
#define	ARMPF_VIC_VAD34		(ARMPF_VIC_BASE + 0x488)		/* 割り込みアドレス格納レジスタ34				*/
#define	ARMPF_VIC_VAD35		(ARMPF_VIC_BASE + 0x48C)		/* 割り込みアドレス格納レジスタ35				*/
#define	ARMPF_VIC_VAD36		(ARMPF_VIC_BASE + 0x490)		/* 割り込みアドレス格納レジスタ36				*/
#define	ARMPF_VIC_VAD37		(ARMPF_VIC_BASE + 0x494)		/* 割り込みアドレス格納レジスタ37				*/
#define	ARMPF_VIC_VAD38		(ARMPF_VIC_BASE + 0x498)		/* 割り込みアドレス格納レジスタ38				*/
#define	ARMPF_VIC_VAD39		(ARMPF_VIC_BASE + 0x49C)		/* 割り込みアドレス格納レジスタ39				*/
#define	ARMPF_VIC_VAD40		(ARMPF_VIC_BASE + 0x4A0)		/* 割り込みアドレス格納レジスタ40				*/
#define	ARMPF_VIC_VAD41		(ARMPF_VIC_BASE + 0x4A4)		/* 割り込みアドレス格納レジスタ41				*/
#define	ARMPF_VIC_VAD42		(ARMPF_VIC_BASE + 0x4A8)		/* 割り込みアドレス格納レジスタ42				*/
#define	ARMPF_VIC_VAD43		(ARMPF_VIC_BASE + 0x4AC)		/* 割り込みアドレス格納レジスタ43				*/
#define	ARMPF_VIC_VAD44		(ARMPF_VIC_BASE + 0x4B0)		/* 割り込みアドレス格納レジスタ44				*/
#define	ARMPF_VIC_VAD45		(ARMPF_VIC_BASE + 0x4B4)		/* 割り込みアドレス格納レジスタ45				*/
#define	ARMPF_VIC_VAD46		(ARMPF_VIC_BASE + 0x4B8)		/* 割り込みアドレス格納レジスタ46				*/
#define	ARMPF_VIC_VAD47		(ARMPF_VIC_BASE + 0x4BC)		/* 割り込みアドレス格納レジスタ47				*/
#define	ARMPF_VIC_VAD48		(ARMPF_VIC_BASE + 0x4C0)		/* 割り込みアドレス格納レジスタ48				*/
#define	ARMPF_VIC_VAD49		(ARMPF_VIC_BASE + 0x4C4)		/* 割り込みアドレス格納レジスタ49				*/
#define	ARMPF_VIC_VAD50		(ARMPF_VIC_BASE + 0x4C8)		/* 割り込みアドレス格納レジスタ50				*/
#define	ARMPF_VIC_VAD51		(ARMPF_VIC_BASE + 0x4CC)		/* 割り込みアドレス格納レジスタ51				*/
#define	ARMPF_VIC_VAD52		(ARMPF_VIC_BASE + 0x4D0)		/* 割り込みアドレス格納レジスタ52				*/
#define	ARMPF_VIC_VAD53		(ARMPF_VIC_BASE + 0x4D4)		/* 割り込みアドレス格納レジスタ53				*/
#define	ARMPF_VIC_VAD54		(ARMPF_VIC_BASE + 0x4D8)		/* 割り込みアドレス格納レジスタ54				*/
#define	ARMPF_VIC_VAD55		(ARMPF_VIC_BASE + 0x4DC)		/* 割り込みアドレス格納レジスタ55				*/
#define	ARMPF_VIC_VAD56		(ARMPF_VIC_BASE + 0x4E0)		/* 割り込みアドレス格納レジスタ56				*/
#define	ARMPF_VIC_VAD57		(ARMPF_VIC_BASE + 0x4E4)		/* 割り込みアドレス格納レジスタ57				*/
#define	ARMPF_VIC_VAD58		(ARMPF_VIC_BASE + 0x4E8)		/* 割り込みアドレス格納レジスタ58				*/
#define	ARMPF_VIC_VAD59		(ARMPF_VIC_BASE + 0x4EC)		/* 割り込みアドレス格納レジスタ59				*/
#define	ARMPF_VIC_VAD60		(ARMPF_VIC_BASE + 0x4F0)		/* 割り込みアドレス格納レジスタ60				*/
#define	ARMPF_VIC_VAD61		(ARMPF_VIC_BASE + 0x4F4)		/* 割り込みアドレス格納レジスタ61				*/
#define	ARMPF_VIC_VAD62		(ARMPF_VIC_BASE + 0x4F8)		/* 割り込みアドレス格納レジスタ62				*/
#define	ARMPF_VIC_VAD63		(ARMPF_VIC_BASE + 0x4FC)		/* 割り込みアドレス格納レジスタ63				*/
//#define	ARMPF_VIC_VAD64		(ARMPF_VIC_BASE + 0x500)		/* 割り込みアドレス格納レジスタ64				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD65		(ARMPF_VIC_BASE + 0x504)		/* 割り込みアドレス格納レジスタ65				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD66		(ARMPF_VIC_BASE + 0x508)		/* 割り込みアドレス格納レジスタ66				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD67		(ARMPF_VIC_BASE + 0x50C)		/* 割り込みアドレス格納レジスタ67				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD68		(ARMPF_VIC_BASE + 0x510)		/* 割り込みアドレス格納レジスタ68				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD69		(ARMPF_VIC_BASE + 0x514)		/* 割り込みアドレス格納レジスタ69				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD70		(ARMPF_VIC_BASE + 0x518)		/* 割り込みアドレス格納レジスタ70				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD71		(ARMPF_VIC_BASE + 0x51C)		/* 割り込みアドレス格納レジスタ71				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD72		(ARMPF_VIC_BASE + 0x520)		/* 割り込みアドレス格納レジスタ72				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD73		(ARMPF_VIC_BASE + 0x524)		/* 割り込みアドレス格納レジスタ73				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD74		(ARMPF_VIC_BASE + 0x528)		/* 割り込みアドレス格納レジスタ74				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD75		(ARMPF_VIC_BASE + 0x52C)		/* 割り込みアドレス格納レジスタ75				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD76		(ARMPF_VIC_BASE + 0x530)		/* 割り込みアドレス格納レジスタ76				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD77		(ARMPF_VIC_BASE + 0x534)		/* 割り込みアドレス格納レジスタ77				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD78		(ARMPF_VIC_BASE + 0x538)		/* 割り込みアドレス格納レジスタ78				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD79		(ARMPF_VIC_BASE + 0x53C)		/* 割り込みアドレス格納レジスタ79				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD80		(ARMPF_VIC_BASE + 0x540)		/* 割り込みアドレス格納レジスタ80				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD81		(ARMPF_VIC_BASE + 0x544)		/* 割り込みアドレス格納レジスタ81				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD82		(ARMPF_VIC_BASE + 0x548)		/* 割り込みアドレス格納レジスタ82				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD83		(ARMPF_VIC_BASE + 0x54C)		/* 割り込みアドレス格納レジスタ83				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD84		(ARMPF_VIC_BASE + 0x550)		/* 割り込みアドレス格納レジスタ84				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD85		(ARMPF_VIC_BASE + 0x554)		/* 割り込みアドレス格納レジスタ85				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD86		(ARMPF_VIC_BASE + 0x558)		/* 割り込みアドレス格納レジスタ86				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD87		(ARMPF_VIC_BASE + 0x55C)		/* 割り込みアドレス格納レジスタ87				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD88		(ARMPF_VIC_BASE + 0x560)		/* 割り込みアドレス格納レジスタ88				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD89		(ARMPF_VIC_BASE + 0x564)		/* 割り込みアドレス格納レジスタ89				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD90		(ARMPF_VIC_BASE + 0x568)		/* 割り込みアドレス格納レジスタ90				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD91		(ARMPF_VIC_BASE + 0x56C)		/* 割り込みアドレス格納レジスタ91				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD92		(ARMPF_VIC_BASE + 0x570)		/* 割り込みアドレス格納レジスタ92				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD93		(ARMPF_VIC_BASE + 0x574)		/* 割り込みアドレス格納レジスタ93				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD94		(ARMPF_VIC_BASE + 0x578)		/* 割り込みアドレス格納レジスタ94				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD95		(ARMPF_VIC_BASE + 0x57C)		/* 割り込みアドレス格納レジスタ95				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD96		(ARMPF_VIC_BASE + 0x580)		/* 割り込みアドレス格納レジスタ96				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD97		(ARMPF_VIC_BASE + 0x584)		/* 割り込みアドレス格納レジスタ97				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD98		(ARMPF_VIC_BASE + 0x588)		/* 割り込みアドレス格納レジスタ98				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD99		(ARMPF_VIC_BASE + 0x58C)		/* 割り込みアドレス格納レジスタ99				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD100	(ARMPF_VIC_BASE + 0x590)		/* 割り込みアドレス格納レジスタ100				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD101	(ARMPF_VIC_BASE + 0x594)		/* 割り込みアドレス格納レジスタ101				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD102	(ARMPF_VIC_BASE + 0x598)		/* 割り込みアドレス格納レジスタ102				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD103	(ARMPF_VIC_BASE + 0x59C)		/* 割り込みアドレス格納レジスタ103				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD104	(ARMPF_VIC_BASE + 0x5A0)		/* 割り込みアドレス格納レジスタ104				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD105	(ARMPF_VIC_BASE + 0x5A4)		/* 割り込みアドレス格納レジスタ105				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD106	(ARMPF_VIC_BASE + 0x5A8)		/* 割り込みアドレス格納レジスタ106				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD107	(ARMPF_VIC_BASE + 0x5AC)		/* 割り込みアドレス格納レジスタ107				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD108	(ARMPF_VIC_BASE + 0x5B0)		/* 割り込みアドレス格納レジスタ108				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD109	(ARMPF_VIC_BASE + 0x5B4)		/* 割り込みアドレス格納レジスタ109				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD110	(ARMPF_VIC_BASE + 0x5B8)		/* 割り込みアドレス格納レジスタ110				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD111	(ARMPF_VIC_BASE + 0x5BC)		/* 割り込みアドレス格納レジスタ111				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD112	(ARMPF_VIC_BASE + 0x5C0)		/* 割り込みアドレス格納レジスタ112				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD113	(ARMPF_VIC_BASE + 0x5C4)		/* 割り込みアドレス格納レジスタ113				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD114	(ARMPF_VIC_BASE + 0x5C8)		/* 割り込みアドレス格納レジスタ114				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD115	(ARMPF_VIC_BASE + 0x5CC)		/* 割り込みアドレス格納レジスタ115				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD116	(ARMPF_VIC_BASE + 0x5D0)		/* 割り込みアドレス格納レジスタ116				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD117	(ARMPF_VIC_BASE + 0x5D4)		/* 割り込みアドレス格納レジスタ117				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD118	(ARMPF_VIC_BASE + 0x5D8)		/* 割り込みアドレス格納レジスタ118				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD119	(ARMPF_VIC_BASE + 0x5DC)		/* 割り込みアドレス格納レジスタ119				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD120	(ARMPF_VIC_BASE + 0x5E0)		/* 割り込みアドレス格納レジスタ120				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD121	(ARMPF_VIC_BASE + 0x5E4)		/* 割り込みアドレス格納レジスタ121				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD122	(ARMPF_VIC_BASE + 0x5E8)		/* 割り込みアドレス格納レジスタ122				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD123	(ARMPF_VIC_BASE + 0x5EC)		/* 割り込みアドレス格納レジスタ123				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD124	(ARMPF_VIC_BASE + 0x5F0)		/* 割り込みアドレス格納レジスタ124				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD125	(ARMPF_VIC_BASE + 0x5F4)		/* 割り込みアドレス格納レジスタ125				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD126	(ARMPF_VIC_BASE + 0x5F8)		/* 割り込みアドレス格納レジスタ126				*//* JL-086使用不可 */
//#define	ARMPF_VIC_VAD127	(ARMPF_VIC_BASE + 0x5FC)		/* 割り込みアドレス格納レジスタ127				*//* JL-086使用不可 */
#define	ARMPF_VIC_PRL0		(ARMPF_VIC_BASE + 0x800)		/* 割り込み優先レベル格納レジスタ0				*/
#define	ARMPF_VIC_PRL1		(ARMPF_VIC_BASE + 0x804)		/* 割り込み優先レベル格納レジスタ1				*/
#define	ARMPF_VIC_PRL2		(ARMPF_VIC_BASE + 0x808)		/* 割り込み優先レベル格納レジスタ2				*/
#define	ARMPF_VIC_PRL3		(ARMPF_VIC_BASE + 0x80C)		/* 割り込み優先レベル格納レジスタ3				*/
#define	ARMPF_VIC_PRL4		(ARMPF_VIC_BASE + 0x810)		/* 割り込み優先レベル格納レジスタ4				*/
#define	ARMPF_VIC_PRL5		(ARMPF_VIC_BASE + 0x814)		/* 割り込み優先レベル格納レジスタ5				*/
#define	ARMPF_VIC_PRL6		(ARMPF_VIC_BASE + 0x818)		/* 割り込み優先レベル格納レジスタ6				*/
#define	ARMPF_VIC_PRL7		(ARMPF_VIC_BASE + 0x81C)		/* 割り込み優先レベル格納レジスタ7				*/
#define	ARMPF_VIC_PRL8		(ARMPF_VIC_BASE + 0x820)		/* 割り込み優先レベル格納レジスタ8				*/
#define	ARMPF_VIC_PRL9		(ARMPF_VIC_BASE + 0x824)		/* 割り込み優先レベル格納レジスタ9				*/
#define	ARMPF_VIC_PRL10		(ARMPF_VIC_BASE + 0x828)		/* 割り込み優先レベル格納レジスタ10				*/
#define	ARMPF_VIC_PRL11		(ARMPF_VIC_BASE + 0x82C)		/* 割り込み優先レベル格納レジスタ11				*/
#define	ARMPF_VIC_PRL12		(ARMPF_VIC_BASE + 0x830)		/* 割り込み優先レベル格納レジスタ12				*/
#define	ARMPF_VIC_PRL13		(ARMPF_VIC_BASE + 0x834)		/* 割り込み優先レベル格納レジスタ13				*/
#define	ARMPF_VIC_PRL14		(ARMPF_VIC_BASE + 0x838)		/* 割り込み優先レベル格納レジスタ14				*/
#define	ARMPF_VIC_PRL15		(ARMPF_VIC_BASE + 0x83C)		/* 割り込み優先レベル格納レジスタ15				*/
#define	ARMPF_VIC_PRL16		(ARMPF_VIC_BASE + 0x840)		/* 割り込み優先レベル格納レジスタ16				*/
#define	ARMPF_VIC_PRL17		(ARMPF_VIC_BASE + 0x844)		/* 割り込み優先レベル格納レジスタ17				*/
#define	ARMPF_VIC_PRL18		(ARMPF_VIC_BASE + 0x848)		/* 割り込み優先レベル格納レジスタ18				*/
#define	ARMPF_VIC_PRL19		(ARMPF_VIC_BASE + 0x84C)		/* 割り込み優先レベル格納レジスタ19				*/
#define	ARMPF_VIC_PRL20		(ARMPF_VIC_BASE + 0x850)		/* 割り込み優先レベル格納レジスタ20				*/
#define	ARMPF_VIC_PRL21		(ARMPF_VIC_BASE + 0x854)		/* 割り込み優先レベル格納レジスタ21				*/
#define	ARMPF_VIC_PRL22		(ARMPF_VIC_BASE + 0x858)		/* 割り込み優先レベル格納レジスタ22				*/
#define	ARMPF_VIC_PRL23		(ARMPF_VIC_BASE + 0x85C)		/* 割り込み優先レベル格納レジスタ23				*/
#define	ARMPF_VIC_PRL24		(ARMPF_VIC_BASE + 0x860)		/* 割り込み優先レベル格納レジスタ24				*/
#define	ARMPF_VIC_PRL25		(ARMPF_VIC_BASE + 0x864)		/* 割り込み優先レベル格納レジスタ25				*/
#define	ARMPF_VIC_PRL26		(ARMPF_VIC_BASE + 0x868)		/* 割り込み優先レベル格納レジスタ26				*/
#define	ARMPF_VIC_PRL27		(ARMPF_VIC_BASE + 0x86C)		/* 割り込み優先レベル格納レジスタ27				*/
#define	ARMPF_VIC_PRL28		(ARMPF_VIC_BASE + 0x870)		/* 割り込み優先レベル格納レジスタ28				*/
#define	ARMPF_VIC_PRL29		(ARMPF_VIC_BASE + 0x874)		/* 割り込み優先レベル格納レジスタ29				*/
#define	ARMPF_VIC_PRL30		(ARMPF_VIC_BASE + 0x878)		/* 割り込み優先レベル格納レジスタ30				*/
#define	ARMPF_VIC_PRL31		(ARMPF_VIC_BASE + 0x87C)		/* 割り込み優先レベル格納レジスタ31				*/
#define	ARMPF_VIC_PRL32		(ARMPF_VIC_BASE + 0x880)		/* 割り込み優先レベル格納レジスタ32				*/
#define	ARMPF_VIC_PRL33		(ARMPF_VIC_BASE + 0x884)		/* 割り込み優先レベル格納レジスタ33				*/
#define	ARMPF_VIC_PRL34		(ARMPF_VIC_BASE + 0x888)		/* 割り込み優先レベル格納レジスタ34				*/
#define	ARMPF_VIC_PRL35		(ARMPF_VIC_BASE + 0x88C)		/* 割り込み優先レベル格納レジスタ35				*/
#define	ARMPF_VIC_PRL36		(ARMPF_VIC_BASE + 0x890)		/* 割り込み優先レベル格納レジスタ36				*/
#define	ARMPF_VIC_PRL37		(ARMPF_VIC_BASE + 0x894)		/* 割り込み優先レベル格納レジスタ37				*/
#define	ARMPF_VIC_PRL38		(ARMPF_VIC_BASE + 0x898)		/* 割り込み優先レベル格納レジスタ38				*/
#define	ARMPF_VIC_PRL39		(ARMPF_VIC_BASE + 0x89C)		/* 割り込み優先レベル格納レジスタ39				*/
#define	ARMPF_VIC_PRL40		(ARMPF_VIC_BASE + 0x8A0)		/* 割り込み優先レベル格納レジスタ40				*/
#define	ARMPF_VIC_PRL41		(ARMPF_VIC_BASE + 0x8A4)		/* 割り込み優先レベル格納レジスタ41				*/
#define	ARMPF_VIC_PRL42		(ARMPF_VIC_BASE + 0x8A8)		/* 割り込み優先レベル格納レジスタ42				*/
#define	ARMPF_VIC_PRL43		(ARMPF_VIC_BASE + 0x8AC)		/* 割り込み優先レベル格納レジスタ43				*/
#define	ARMPF_VIC_PRL44		(ARMPF_VIC_BASE + 0x8B0)		/* 割り込み優先レベル格納レジスタ44				*/
#define	ARMPF_VIC_PRL45		(ARMPF_VIC_BASE + 0x8B4)		/* 割り込み優先レベル格納レジスタ45				*/
#define	ARMPF_VIC_PRL46		(ARMPF_VIC_BASE + 0x8B8)		/* 割り込み優先レベル格納レジスタ46				*/
#define	ARMPF_VIC_PRL47		(ARMPF_VIC_BASE + 0x8BC)		/* 割り込み優先レベル格納レジスタ47				*/
#define	ARMPF_VIC_PRL48		(ARMPF_VIC_BASE + 0x8C0)		/* 割り込み優先レベル格納レジスタ48				*/
#define	ARMPF_VIC_PRL49		(ARMPF_VIC_BASE + 0x8C4)		/* 割り込み優先レベル格納レジスタ49				*/
#define	ARMPF_VIC_PRL50		(ARMPF_VIC_BASE + 0x8C8)		/* 割り込み優先レベル格納レジスタ50				*/
#define	ARMPF_VIC_PRL51		(ARMPF_VIC_BASE + 0x8CC)		/* 割り込み優先レベル格納レジスタ51				*/
#define	ARMPF_VIC_PRL52		(ARMPF_VIC_BASE + 0x8D0)		/* 割り込み優先レベル格納レジスタ52				*/
#define	ARMPF_VIC_PRL53		(ARMPF_VIC_BASE + 0x8D4)		/* 割り込み優先レベル格納レジスタ53				*/
#define	ARMPF_VIC_PRL54		(ARMPF_VIC_BASE + 0x8D8)		/* 割り込み優先レベル格納レジスタ54				*/
#define	ARMPF_VIC_PRL55		(ARMPF_VIC_BASE + 0x8DC)		/* 割り込み優先レベル格納レジスタ55				*/
#define	ARMPF_VIC_PRL56		(ARMPF_VIC_BASE + 0x8E0)		/* 割り込み優先レベル格納レジスタ56				*/
#define	ARMPF_VIC_PRL57		(ARMPF_VIC_BASE + 0x8E4)		/* 割り込み優先レベル格納レジスタ57				*/
#define	ARMPF_VIC_PRL58		(ARMPF_VIC_BASE + 0x8E8)		/* 割り込み優先レベル格納レジスタ58				*/
#define	ARMPF_VIC_PRL59		(ARMPF_VIC_BASE + 0x8EC)		/* 割り込み優先レベル格納レジスタ59				*/
#define	ARMPF_VIC_PRL60		(ARMPF_VIC_BASE + 0x8F0)		/* 割り込み優先レベル格納レジスタ60				*/
#define	ARMPF_VIC_PRL61		(ARMPF_VIC_BASE + 0x8F4)		/* 割り込み優先レベル格納レジスタ61				*/
#define	ARMPF_VIC_PRL62		(ARMPF_VIC_BASE + 0x8F8)		/* 割り込み優先レベル格納レジスタ62				*/
#define	ARMPF_VIC_PRL63		(ARMPF_VIC_BASE + 0x8FC)		/* 割り込み優先レベル格納レジスタ63				*/
//#define	ARMPF_VIC_PRL64		(ARMPF_VIC_BASE + 0x900)		/* 割り込み優先レベル格納レジスタ64				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL65		(ARMPF_VIC_BASE + 0x904)		/* 割り込み優先レベル格納レジスタ65				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL66		(ARMPF_VIC_BASE + 0x908)		/* 割り込み優先レベル格納レジスタ66				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL67		(ARMPF_VIC_BASE + 0x90C)		/* 割り込み優先レベル格納レジスタ67				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL68		(ARMPF_VIC_BASE + 0x910)		/* 割り込み優先レベル格納レジスタ68				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL69		(ARMPF_VIC_BASE + 0x914)		/* 割り込み優先レベル格納レジスタ69				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL70		(ARMPF_VIC_BASE + 0x918)		/* 割り込み優先レベル格納レジスタ70				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL71		(ARMPF_VIC_BASE + 0x91C)		/* 割り込み優先レベル格納レジスタ71				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL72		(ARMPF_VIC_BASE + 0x920)		/* 割り込み優先レベル格納レジスタ72				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL73		(ARMPF_VIC_BASE + 0x924)		/* 割り込み優先レベル格納レジスタ73				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL74		(ARMPF_VIC_BASE + 0x928)		/* 割り込み優先レベル格納レジスタ74				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL75		(ARMPF_VIC_BASE + 0x92C)		/* 割り込み優先レベル格納レジスタ75				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL76		(ARMPF_VIC_BASE + 0x930)		/* 割り込み優先レベル格納レジスタ76				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL77		(ARMPF_VIC_BASE + 0x934)		/* 割り込み優先レベル格納レジスタ77				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL78		(ARMPF_VIC_BASE + 0x938)		/* 割り込み優先レベル格納レジスタ78				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL79		(ARMPF_VIC_BASE + 0x93C)		/* 割り込み優先レベル格納レジスタ79				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL80		(ARMPF_VIC_BASE + 0x940)		/* 割り込み優先レベル格納レジスタ80				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL81		(ARMPF_VIC_BASE + 0x944)		/* 割り込み優先レベル格納レジスタ81				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL82		(ARMPF_VIC_BASE + 0x948)		/* 割り込み優先レベル格納レジスタ82				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL83		(ARMPF_VIC_BASE + 0x94C)		/* 割り込み優先レベル格納レジスタ83				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL84		(ARMPF_VIC_BASE + 0x950)		/* 割り込み優先レベル格納レジスタ84				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL85		(ARMPF_VIC_BASE + 0x954)		/* 割り込み優先レベル格納レジスタ85				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL86		(ARMPF_VIC_BASE + 0x958)		/* 割り込み優先レベル格納レジスタ86				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL87		(ARMPF_VIC_BASE + 0x95C)		/* 割り込み優先レベル格納レジスタ87				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL88		(ARMPF_VIC_BASE + 0x960)		/* 割り込み優先レベル格納レジスタ88				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL89		(ARMPF_VIC_BASE + 0x964)		/* 割り込み優先レベル格納レジスタ89				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL90		(ARMPF_VIC_BASE + 0x968)		/* 割り込み優先レベル格納レジスタ90				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL91		(ARMPF_VIC_BASE + 0x96C)		/* 割り込み優先レベル格納レジスタ91				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL92		(ARMPF_VIC_BASE + 0x970)		/* 割り込み優先レベル格納レジスタ92				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL93		(ARMPF_VIC_BASE + 0x974)		/* 割り込み優先レベル格納レジスタ93				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL94		(ARMPF_VIC_BASE + 0x978)		/* 割り込み優先レベル格納レジスタ94				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL95		(ARMPF_VIC_BASE + 0x97C)		/* 割り込み優先レベル格納レジスタ95				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL96		(ARMPF_VIC_BASE + 0x980)		/* 割り込み優先レベル格納レジスタ96				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL97		(ARMPF_VIC_BASE + 0x984)		/* 割り込み優先レベル格納レジスタ97				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL98		(ARMPF_VIC_BASE + 0x988)		/* 割り込み優先レベル格納レジスタ98				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL99		(ARMPF_VIC_BASE + 0x98C)		/* 割り込み優先レベル格納レジスタ99				*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL100	(ARMPF_VIC_BASE + 0x990)		/* 割り込み優先レベル格納レジスタ100			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL101	(ARMPF_VIC_BASE + 0x994)		/* 割り込み優先レベル格納レジスタ101			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL102	(ARMPF_VIC_BASE + 0x998)		/* 割り込み優先レベル格納レジスタ102			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL103	(ARMPF_VIC_BASE + 0x99C)		/* 割り込み優先レベル格納レジスタ103			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL104	(ARMPF_VIC_BASE + 0x9A0)		/* 割り込み優先レベル格納レジスタ104			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL105	(ARMPF_VIC_BASE + 0x9A4)		/* 割り込み優先レベル格納レジスタ105			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL106	(ARMPF_VIC_BASE + 0x9A8)		/* 割り込み優先レベル格納レジスタ106			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL107	(ARMPF_VIC_BASE + 0x9AC)		/* 割り込み優先レベル格納レジスタ107			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL108	(ARMPF_VIC_BASE + 0x9B0)		/* 割り込み優先レベル格納レジスタ108			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL109	(ARMPF_VIC_BASE + 0x9B4)		/* 割り込み優先レベル格納レジスタ109			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL110	(ARMPF_VIC_BASE + 0x9B8)		/* 割り込み優先レベル格納レジスタ110			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL111	(ARMPF_VIC_BASE + 0x9BC)		/* 割り込み優先レベル格納レジスタ111			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL112	(ARMPF_VIC_BASE + 0x9C0)		/* 割り込み優先レベル格納レジスタ112			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL113	(ARMPF_VIC_BASE + 0x9C4)		/* 割り込み優先レベル格納レジスタ113			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL114	(ARMPF_VIC_BASE + 0x9C8)		/* 割り込み優先レベル格納レジスタ114			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL115	(ARMPF_VIC_BASE + 0x9CC)		/* 割り込み優先レベル格納レジスタ115			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL116	(ARMPF_VIC_BASE + 0x9D0)		/* 割り込み優先レベル格納レジスタ116			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL117	(ARMPF_VIC_BASE + 0x9D4)		/* 割り込み優先レベル格納レジスタ117			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL118	(ARMPF_VIC_BASE + 0x9D8)		/* 割り込み優先レベル格納レジスタ118			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL119	(ARMPF_VIC_BASE + 0x9DC)		/* 割り込み優先レベル格納レジスタ119			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL120	(ARMPF_VIC_BASE + 0x9E0)		/* 割り込み優先レベル格納レジスタ120			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL121	(ARMPF_VIC_BASE + 0x9E4)		/* 割り込み優先レベル格納レジスタ121			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL122	(ARMPF_VIC_BASE + 0x9E8)		/* 割り込み優先レベル格納レジスタ122			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL123	(ARMPF_VIC_BASE + 0x9EC)		/* 割り込み優先レベル格納レジスタ123			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL124	(ARMPF_VIC_BASE + 0x9F0)		/* 割り込み優先レベル格納レジスタ124			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL125	(ARMPF_VIC_BASE + 0x9F4)		/* 割り込み優先レベル格納レジスタ125			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL126	(ARMPF_VIC_BASE + 0x9F8)		/* 割り込み優先レベル格納レジスタ126			*//* JL-086使用不可 */
//#define	ARMPF_VIC_PRL127	(ARMPF_VIC_BASE + 0x9FC)		/* 割り込み優先レベル格納レジスタ127			*//* JL-086使用不可 */
#define	ARMPF_VIC_TCR		(ARMPF_VIC_BASE + 0xC00)		/* テスト・モード選択レジスタ					*/
#define	ARMPF_VIC_TICR		(ARMPF_VIC_BASE + 0xC04)		/* テスト・モード割り込み入力制御レジスタ		*/
#define	ARMPF_VIC_TACR		(ARMPF_VIC_BASE + 0xC08)		/* テスト・モード割り込みアドレス制御レジスタ	*/
#define	ARMPF_VIC_IOS		(ARMPF_VIC_BASE + 0xC0C)		/* 割り込み要求ステータス・レジスタ				*/
#define	ARMPF_VIC_VAOS		(ARMPF_VIC_BASE + 0xC10)		/* 割り込み要求アドレス・ステータス・レジスタ	*/

#endif
/*-------end of file----------------------------------------------------------------------------------------*/
