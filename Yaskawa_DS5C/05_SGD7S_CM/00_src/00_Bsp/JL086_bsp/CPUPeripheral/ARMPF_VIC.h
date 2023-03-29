/************************************************************************************************************/
/*																											*/
/*																											*/
/*		ARMPF_VIC.h : MercurySoC �y���t�F���� VIC�w�b�_														*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		���荞�݃R���g���[��(VIC)�֘A�w�b�_�t�@�C��															*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.14  Y.Tanaka		����															*/
/*																											*/
/************************************************************************************************************/
#include	"ARMPF.h"


#ifndef	_ARMPF_VIC_H_
#define	_ARMPF_VIC_H_


#define	ARMPF_VIC_BASE	0xEFFDF000							/* VIC�x�[�X�A�h���X							*/

/*----------------------------------------------------------------------------------------------------------*/
/*		���荞�ݎ��s���I��																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_IRQ		0x00000000						/* IRQ���荞��									*/
#define	ARMPF_VIC_FIQ		0x00000001						/* FIQ���荞��									*/

/*----------------------------------------------------------------------------------------------------------*/
/*		���荞�݌��o���@�I��	PLS																			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_LVL		0x00							/* ���x�����o									*/
#define	ARMPF_VIC_EDGE		0x01							/* �G�b�W���o									*/

/*----------------------------------------------------------------------------------------------------------*/
/*		���荞�݌��o�L�����x���I��	LVLC0-7																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_HIGH		0x01							/* �n�C���x���Ō��o								*/
#define	ARMPF_VIC_LOW 		0x02							/* ���[���x���Ō��o								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		���荞�݌��o�L���G�b�W�I��	EDGC0-7																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_RISE		0x01							/* �����オ��Ō��o								*/
#define	ARMPF_VIC_FALL		0x02							/* ������Ō��o									*/
#define	ARMPF_VIC_BIEDGE	0x03							/* �����G�b�W�Ō��o								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		���荞�ݗD��x	PRL0-127																			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_PRIORYTY0		0							/* �D��x0�iMAX�j								*/
#define	ARMPF_VIC_PRIORYTY1		1							/* �D��x1										*/
#define	ARMPF_VIC_PRIORYTY2		2							/* �D��x2										*/
#define	ARMPF_VIC_PRIORYTY3		3							/* �D��x3										*/
#define	ARMPF_VIC_PRIORYTY4		4							/* �D��x4										*/
#define	ARMPF_VIC_PRIORYTY5		5							/* �D��x5										*/
#define	ARMPF_VIC_PRIORYTY6		6							/* �D��x6										*/
#define	ARMPF_VIC_PRIORYTY7		7							/* �D��x7										*/
#define	ARMPF_VIC_PRIORYTY8		8							/* �D��x8										*/
#define	ARMPF_VIC_PRIORYTY9		9							/* �D��x9										*/
#define	ARMPF_VIC_PRIORYTY10	10							/* �D��x10										*/
#define	ARMPF_VIC_PRIORYTY11	11							/* �D��x11										*/
#define	ARMPF_VIC_PRIORYTY12	12							/* �D��x12										*/
#define	ARMPF_VIC_PRIORYTY13	13							/* �D��x13										*/
#define	ARMPF_VIC_PRIORYTY14	14							/* �D��x14										*/
#define	ARMPF_VIC_PRIORYTY15	15							/* �D��x15�iMIN�j								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		VIC register																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_VIC_IRQS0		(ARMPF_VIC_BASE + 0x000)		/* IRQ�X�e�[�^�X�E���W�X�^0						*/
#define	ARMPF_VIC_IRQS1		(ARMPF_VIC_BASE + 0x004)		/* IRQ�X�e�[�^�X�E���W�X�^1						*/
//#define	ARMPF_VIC_IRQS2		(ARMPF_VIC_BASE + 0x008)		/* IRQ�X�e�[�^�X�E���W�X�^2						*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_IRQS3		(ARMPF_VIC_BASE + 0x00C)		/* IRQ�X�e�[�^�X�E���W�X�^3						*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_FIQS0		(ARMPF_VIC_BASE + 0x020)		/* FIQ�X�e�[�^�X�E���W�X�^0						*/
#define	ARMPF_VIC_FIQS1		(ARMPF_VIC_BASE + 0x024)		/* FIQ�X�e�[�^�X�E���W�X�^1						*/
//#define	ARMPF_VIC_FIQS2		(ARMPF_VIC_BASE + 0x028)		/* FIQ�X�e�[�^�X�E���W�X�^2						*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_FIQS3		(ARMPF_VIC_BASE + 0x02C)		/* FIQ�X�e�[�^�X�E���W�X�^3						*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_RAIS0		(ARMPF_VIC_BASE + 0x040)		/* ���荞�ݓ��̓X�e�[�^�X�E���W�X�^0			*/
#define	ARMPF_VIC_RAIS1		(ARMPF_VIC_BASE + 0x044)		/* ���荞�ݓ��̓X�e�[�^�X�E���W�X�^1			*/
//#define	ARMPF_VIC_RAIS2		(ARMPF_VIC_BASE + 0x048)		/* ���荞�ݓ��̓X�e�[�^�X�E���W�X�^2			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_RAIS3		(ARMPF_VIC_BASE + 0x04C)		/* ���荞�ݓ��̓X�e�[�^�X�E���W�X�^3			*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_ISL0		(ARMPF_VIC_BASE + 0x060)		/* IRQ/FIQ���荞�ݑI�����W�X�^0					*/
#define	ARMPF_VIC_ISL1		(ARMPF_VIC_BASE + 0x064)		/* IRQ/FIQ���荞�ݑI�����W�X�^1					*/
//#define	ARMPF_VIC_ISL2		(ARMPF_VIC_BASE + 0x068)		/* IRQ/FIQ���荞�ݑI�����W�X�^2					*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_ISL3		(ARMPF_VIC_BASE + 0x06C)		/* IRQ/FIQ���荞�ݑI�����W�X�^3					*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_IEN0		(ARMPF_VIC_BASE + 0x080)		/* ���荞�݃C�l�[�u���E���W�X�^0				*/
#define	ARMPF_VIC_IEN1		(ARMPF_VIC_BASE + 0x084)		/* ���荞�݃C�l�[�u���E���W�X�^1				*/
//#define	ARMPF_VIC_IEN2		(ARMPF_VIC_BASE + 0x088)		/* ���荞�݃C�l�[�u���E���W�X�^2				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_IEN3		(ARMPF_VIC_BASE + 0x08C)		/* ���荞�݃C�l�[�u���E���W�X�^3				*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_IEC0		(ARMPF_VIC_BASE + 0x0A0)		/* ���荞�݃C�l�[�u���E�N���A�E���W�X�^0		*/
#define	ARMPF_VIC_IEC1		(ARMPF_VIC_BASE + 0x0A4)		/* ���荞�݃C�l�[�u���E�N���A�E���W�X�^1		*/
//#define	ARMPF_VIC_IEC2		(ARMPF_VIC_BASE + 0x0A8)		/* ���荞�݃C�l�[�u���E�N���A�E���W�X�^2		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_IEC3		(ARMPF_VIC_BASE + 0x0AC)		/* ���荞�݃C�l�[�u���E�N���A�E���W�X�^3		*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_SWI0		(ARMPF_VIC_BASE + 0x0C0)		/* �\�t�g�E�F�A���荞�݃��W�X�^0				*/
#define	ARMPF_VIC_SWI1		(ARMPF_VIC_BASE + 0x0C4)		/* �\�t�g�E�F�A���荞�݃��W�X�^1				*/
//#define	ARMPF_VIC_SWI2		(ARMPF_VIC_BASE + 0x0C8)		/* �\�t�g�E�F�A���荞�݃��W�X�^2				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_SWI3		(ARMPF_VIC_BASE + 0x0CC)		/* �\�t�g�E�F�A���荞�݃��W�X�^3				*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_SWC0		(ARMPF_VIC_BASE + 0x0E0)		/* �\�t�g�E�F�A���荞�݃N���A�E���W�X�^0		*/
#define	ARMPF_VIC_SWC1		(ARMPF_VIC_BASE + 0x0E4)		/* �\�t�g�E�F�A���荞�݃N���A�E���W�X�^1		*/
//#define	ARMPF_VIC_SWC2		(ARMPF_VIC_BASE + 0x0E8)		/* �\�t�g�E�F�A���荞�݃N���A�E���W�X�^2		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_SWC3		(ARMPF_VIC_BASE + 0x0EC)		/* �\�t�g�E�F�A���荞�݃N���A�E���W�X�^3		*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_PLS0		(ARMPF_VIC_BASE + 0x100)		/* ���荞�݌��o�^�C�v�I�����W�X�^0				*/
#define	ARMPF_VIC_PLS1		(ARMPF_VIC_BASE + 0x104)		/* ���荞�݌��o�^�C�v�I�����W�X�^1				*/
//#define	ARMPF_VIC_PLS2		(ARMPF_VIC_BASE + 0x108)		/* ���荞�݌��o�^�C�v�I�����W�X�^2				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PLS3		(ARMPF_VIC_BASE + 0x10C)		/* ���荞�݌��o�^�C�v�I�����W�X�^3				*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_PIC0		(ARMPF_VIC_BASE + 0x120)		/* �G�b�W���o�r�b�g�E�N���A�E���W�X�^0			*/
#define	ARMPF_VIC_PIC1		(ARMPF_VIC_BASE + 0x124)		/* �G�b�W���o�r�b�g�E�N���A�E���W�X�^1			*/
//#define	ARMPF_VIC_PIC2		(ARMPF_VIC_BASE + 0x128)		/* �G�b�W���o�r�b�g�E�N���A�E���W�X�^2			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PIC3		(ARMPF_VIC_BASE + 0x12C)		/* �G�b�W���o�r�b�g�E�N���A�E���W�X�^3			*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_EDGC0		(ARMPF_VIC_BASE + 0x140)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^0		*/
#define	ARMPF_VIC_EDGC1		(ARMPF_VIC_BASE + 0x144)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^1		*/
#define	ARMPF_VIC_EDGC2		(ARMPF_VIC_BASE + 0x148)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^2		*/
#define	ARMPF_VIC_EDGC3		(ARMPF_VIC_BASE + 0x14C)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^3		*/
//#define	ARMPF_VIC_EDGC4		(ARMPF_VIC_BASE + 0x150)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^4		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_EDGC5		(ARMPF_VIC_BASE + 0x154)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^5		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_EDGC6		(ARMPF_VIC_BASE + 0x158)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^6		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_EDGC7		(ARMPF_VIC_BASE + 0x15C)		/* ���荞�݃G�b�W�E�R���g���[���E���W�X�^7		*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_LVLC0		(ARMPF_VIC_BASE + 0x180)		/* ���荞�݃��x���E�R���g���[���E���W�X�^0		*/
#define	ARMPF_VIC_LVLC1		(ARMPF_VIC_BASE + 0x184)		/* ���荞�݃��x���E�R���g���[���E���W�X�^1		*/
#define	ARMPF_VIC_LVLC2		(ARMPF_VIC_BASE + 0x188)		/* ���荞�݃��x���E�R���g���[���E���W�X�^2		*/
#define	ARMPF_VIC_LVLC3		(ARMPF_VIC_BASE + 0x18C)		/* ���荞�݃��x���E�R���g���[���E���W�X�^3		*/
//#define	ARMPF_VIC_LVLC4		(ARMPF_VIC_BASE + 0x190)		/* ���荞�݃��x���E�R���g���[���E���W�X�^4		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_LVLC5		(ARMPF_VIC_BASE + 0x194)		/* ���荞�݃��x���E�R���g���[���E���W�X�^5		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_LVLC6		(ARMPF_VIC_BASE + 0x198)		/* ���荞�݃��x���E�R���g���[���E���W�X�^6		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_LVLC7		(ARMPF_VIC_BASE + 0x19C)		/* ���荞�݃��x���E�R���g���[���E���W�X�^7		*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_PRLM		(ARMPF_VIC_BASE + 0x1C0)		/* ���荞�ݗD�惌�x���E�}�X�N�E���W�X�^			*/
#define	ARMPF_VIC_PRLC		(ARMPF_VIC_BASE + 0x1C4)		/* ���荞�ݗD�惌�x���E�}�X�N�E�N���A�E���W�X�^	*/
#define	ARMPF_VIC_UEN 		(ARMPF_VIC_BASE + 0x1C8)		/* ���[�U�[���[�h�E�C�l�[�u���E���W�X�^			*/
#define	ARMPF_VIC_HVA 		(ARMPF_VIC_BASE + 0x200)		/* ���荞�݃A�h���X�E���W�X�^					*/
#define	ARMPF_VIC_ISS0 		(ARMPF_VIC_BASE + 0x210)		/* ���荞�݃T�[�r�X�E�X�e�[�^�X�E���W�X�^0		*/
#define	ARMPF_VIC_ISS1 		(ARMPF_VIC_BASE + 0x214)		/* ���荞�݃T�[�r�X�E�X�e�[�^�X�E���W�X�^1		*/
//#define	ARMPF_VIC_ISS2 		(ARMPF_VIC_BASE + 0x218)		/* ���荞�݃T�[�r�X�E�X�e�[�^�X�E���W�X�^2		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_ISS3 		(ARMPF_VIC_BASE + 0x21C)		/* ���荞�݃T�[�r�X�E�X�e�[�^�X�E���W�X�^3		*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_ISC0 		(ARMPF_VIC_BASE + 0x230)		/* ���荞�݃T�[�r�X�E�J�����g�E���W�X�^0		*/
#define	ARMPF_VIC_ISC1 		(ARMPF_VIC_BASE + 0x234)		/* ���荞�݃T�[�r�X�E�J�����g�E���W�X�^1		*/
//#define	ARMPF_VIC_ISC2 		(ARMPF_VIC_BASE + 0x238)		/* ���荞�݃T�[�r�X�E�J�����g�E���W�X�^2		*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_ISC3 		(ARMPF_VIC_BASE + 0x23C)		/* ���荞�݃T�[�r�X�E�J�����g�E���W�X�^3		*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_VAD0		(ARMPF_VIC_BASE + 0x400)		/* ���荞�݃A�h���X�i�[���W�X�^0				*/
#define	ARMPF_VIC_VAD1		(ARMPF_VIC_BASE + 0x404)		/* ���荞�݃A�h���X�i�[���W�X�^1				*/
#define	ARMPF_VIC_VAD2		(ARMPF_VIC_BASE + 0x408)		/* ���荞�݃A�h���X�i�[���W�X�^2				*/
#define	ARMPF_VIC_VAD3		(ARMPF_VIC_BASE + 0x40C)		/* ���荞�݃A�h���X�i�[���W�X�^3				*/
#define	ARMPF_VIC_VAD4		(ARMPF_VIC_BASE + 0x410)		/* ���荞�݃A�h���X�i�[���W�X�^4				*/
#define	ARMPF_VIC_VAD5		(ARMPF_VIC_BASE + 0x414)		/* ���荞�݃A�h���X�i�[���W�X�^5				*/
#define	ARMPF_VIC_VAD6		(ARMPF_VIC_BASE + 0x418)		/* ���荞�݃A�h���X�i�[���W�X�^6				*/
#define	ARMPF_VIC_VAD7		(ARMPF_VIC_BASE + 0x41C)		/* ���荞�݃A�h���X�i�[���W�X�^7				*/
#define	ARMPF_VIC_VAD8		(ARMPF_VIC_BASE + 0x420)		/* ���荞�݃A�h���X�i�[���W�X�^8				*/
#define	ARMPF_VIC_VAD9		(ARMPF_VIC_BASE + 0x424)		/* ���荞�݃A�h���X�i�[���W�X�^9				*/
#define	ARMPF_VIC_VAD10		(ARMPF_VIC_BASE + 0x428)		/* ���荞�݃A�h���X�i�[���W�X�^10				*/
#define	ARMPF_VIC_VAD11		(ARMPF_VIC_BASE + 0x42C)		/* ���荞�݃A�h���X�i�[���W�X�^11				*/
#define	ARMPF_VIC_VAD12		(ARMPF_VIC_BASE + 0x430)		/* ���荞�݃A�h���X�i�[���W�X�^12				*/
#define	ARMPF_VIC_VAD13		(ARMPF_VIC_BASE + 0x434)		/* ���荞�݃A�h���X�i�[���W�X�^13				*/
#define	ARMPF_VIC_VAD14		(ARMPF_VIC_BASE + 0x438)		/* ���荞�݃A�h���X�i�[���W�X�^14				*/
#define	ARMPF_VIC_VAD15		(ARMPF_VIC_BASE + 0x43C)		/* ���荞�݃A�h���X�i�[���W�X�^15				*/
#define	ARMPF_VIC_VAD16		(ARMPF_VIC_BASE + 0x440)		/* ���荞�݃A�h���X�i�[���W�X�^16				*/
#define	ARMPF_VIC_VAD17		(ARMPF_VIC_BASE + 0x444)		/* ���荞�݃A�h���X�i�[���W�X�^17				*/
#define	ARMPF_VIC_VAD18		(ARMPF_VIC_BASE + 0x448)		/* ���荞�݃A�h���X�i�[���W�X�^18				*/
#define	ARMPF_VIC_VAD19		(ARMPF_VIC_BASE + 0x44C)		/* ���荞�݃A�h���X�i�[���W�X�^19				*/
#define	ARMPF_VIC_VAD20		(ARMPF_VIC_BASE + 0x450)		/* ���荞�݃A�h���X�i�[���W�X�^20				*/
#define	ARMPF_VIC_VAD21		(ARMPF_VIC_BASE + 0x454)		/* ���荞�݃A�h���X�i�[���W�X�^21				*/
#define	ARMPF_VIC_VAD22		(ARMPF_VIC_BASE + 0x458)		/* ���荞�݃A�h���X�i�[���W�X�^22				*/
#define	ARMPF_VIC_VAD23		(ARMPF_VIC_BASE + 0x45C)		/* ���荞�݃A�h���X�i�[���W�X�^23				*/
#define	ARMPF_VIC_VAD24		(ARMPF_VIC_BASE + 0x460)		/* ���荞�݃A�h���X�i�[���W�X�^24				*/
#define	ARMPF_VIC_VAD25		(ARMPF_VIC_BASE + 0x464)		/* ���荞�݃A�h���X�i�[���W�X�^25				*/
#define	ARMPF_VIC_VAD26		(ARMPF_VIC_BASE + 0x468)		/* ���荞�݃A�h���X�i�[���W�X�^26				*/
#define	ARMPF_VIC_VAD27		(ARMPF_VIC_BASE + 0x46C)		/* ���荞�݃A�h���X�i�[���W�X�^27				*/
#define	ARMPF_VIC_VAD28		(ARMPF_VIC_BASE + 0x470)		/* ���荞�݃A�h���X�i�[���W�X�^28				*/
#define	ARMPF_VIC_VAD29		(ARMPF_VIC_BASE + 0x474)		/* ���荞�݃A�h���X�i�[���W�X�^29				*/
#define	ARMPF_VIC_VAD30		(ARMPF_VIC_BASE + 0x478)		/* ���荞�݃A�h���X�i�[���W�X�^30				*/
#define	ARMPF_VIC_VAD31		(ARMPF_VIC_BASE + 0x47C)		/* ���荞�݃A�h���X�i�[���W�X�^31				*/
#define	ARMPF_VIC_VAD32		(ARMPF_VIC_BASE + 0x480)		/* ���荞�݃A�h���X�i�[���W�X�^32				*/
#define	ARMPF_VIC_VAD33		(ARMPF_VIC_BASE + 0x484)		/* ���荞�݃A�h���X�i�[���W�X�^33				*/
#define	ARMPF_VIC_VAD34		(ARMPF_VIC_BASE + 0x488)		/* ���荞�݃A�h���X�i�[���W�X�^34				*/
#define	ARMPF_VIC_VAD35		(ARMPF_VIC_BASE + 0x48C)		/* ���荞�݃A�h���X�i�[���W�X�^35				*/
#define	ARMPF_VIC_VAD36		(ARMPF_VIC_BASE + 0x490)		/* ���荞�݃A�h���X�i�[���W�X�^36				*/
#define	ARMPF_VIC_VAD37		(ARMPF_VIC_BASE + 0x494)		/* ���荞�݃A�h���X�i�[���W�X�^37				*/
#define	ARMPF_VIC_VAD38		(ARMPF_VIC_BASE + 0x498)		/* ���荞�݃A�h���X�i�[���W�X�^38				*/
#define	ARMPF_VIC_VAD39		(ARMPF_VIC_BASE + 0x49C)		/* ���荞�݃A�h���X�i�[���W�X�^39				*/
#define	ARMPF_VIC_VAD40		(ARMPF_VIC_BASE + 0x4A0)		/* ���荞�݃A�h���X�i�[���W�X�^40				*/
#define	ARMPF_VIC_VAD41		(ARMPF_VIC_BASE + 0x4A4)		/* ���荞�݃A�h���X�i�[���W�X�^41				*/
#define	ARMPF_VIC_VAD42		(ARMPF_VIC_BASE + 0x4A8)		/* ���荞�݃A�h���X�i�[���W�X�^42				*/
#define	ARMPF_VIC_VAD43		(ARMPF_VIC_BASE + 0x4AC)		/* ���荞�݃A�h���X�i�[���W�X�^43				*/
#define	ARMPF_VIC_VAD44		(ARMPF_VIC_BASE + 0x4B0)		/* ���荞�݃A�h���X�i�[���W�X�^44				*/
#define	ARMPF_VIC_VAD45		(ARMPF_VIC_BASE + 0x4B4)		/* ���荞�݃A�h���X�i�[���W�X�^45				*/
#define	ARMPF_VIC_VAD46		(ARMPF_VIC_BASE + 0x4B8)		/* ���荞�݃A�h���X�i�[���W�X�^46				*/
#define	ARMPF_VIC_VAD47		(ARMPF_VIC_BASE + 0x4BC)		/* ���荞�݃A�h���X�i�[���W�X�^47				*/
#define	ARMPF_VIC_VAD48		(ARMPF_VIC_BASE + 0x4C0)		/* ���荞�݃A�h���X�i�[���W�X�^48				*/
#define	ARMPF_VIC_VAD49		(ARMPF_VIC_BASE + 0x4C4)		/* ���荞�݃A�h���X�i�[���W�X�^49				*/
#define	ARMPF_VIC_VAD50		(ARMPF_VIC_BASE + 0x4C8)		/* ���荞�݃A�h���X�i�[���W�X�^50				*/
#define	ARMPF_VIC_VAD51		(ARMPF_VIC_BASE + 0x4CC)		/* ���荞�݃A�h���X�i�[���W�X�^51				*/
#define	ARMPF_VIC_VAD52		(ARMPF_VIC_BASE + 0x4D0)		/* ���荞�݃A�h���X�i�[���W�X�^52				*/
#define	ARMPF_VIC_VAD53		(ARMPF_VIC_BASE + 0x4D4)		/* ���荞�݃A�h���X�i�[���W�X�^53				*/
#define	ARMPF_VIC_VAD54		(ARMPF_VIC_BASE + 0x4D8)		/* ���荞�݃A�h���X�i�[���W�X�^54				*/
#define	ARMPF_VIC_VAD55		(ARMPF_VIC_BASE + 0x4DC)		/* ���荞�݃A�h���X�i�[���W�X�^55				*/
#define	ARMPF_VIC_VAD56		(ARMPF_VIC_BASE + 0x4E0)		/* ���荞�݃A�h���X�i�[���W�X�^56				*/
#define	ARMPF_VIC_VAD57		(ARMPF_VIC_BASE + 0x4E4)		/* ���荞�݃A�h���X�i�[���W�X�^57				*/
#define	ARMPF_VIC_VAD58		(ARMPF_VIC_BASE + 0x4E8)		/* ���荞�݃A�h���X�i�[���W�X�^58				*/
#define	ARMPF_VIC_VAD59		(ARMPF_VIC_BASE + 0x4EC)		/* ���荞�݃A�h���X�i�[���W�X�^59				*/
#define	ARMPF_VIC_VAD60		(ARMPF_VIC_BASE + 0x4F0)		/* ���荞�݃A�h���X�i�[���W�X�^60				*/
#define	ARMPF_VIC_VAD61		(ARMPF_VIC_BASE + 0x4F4)		/* ���荞�݃A�h���X�i�[���W�X�^61				*/
#define	ARMPF_VIC_VAD62		(ARMPF_VIC_BASE + 0x4F8)		/* ���荞�݃A�h���X�i�[���W�X�^62				*/
#define	ARMPF_VIC_VAD63		(ARMPF_VIC_BASE + 0x4FC)		/* ���荞�݃A�h���X�i�[���W�X�^63				*/
//#define	ARMPF_VIC_VAD64		(ARMPF_VIC_BASE + 0x500)		/* ���荞�݃A�h���X�i�[���W�X�^64				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD65		(ARMPF_VIC_BASE + 0x504)		/* ���荞�݃A�h���X�i�[���W�X�^65				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD66		(ARMPF_VIC_BASE + 0x508)		/* ���荞�݃A�h���X�i�[���W�X�^66				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD67		(ARMPF_VIC_BASE + 0x50C)		/* ���荞�݃A�h���X�i�[���W�X�^67				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD68		(ARMPF_VIC_BASE + 0x510)		/* ���荞�݃A�h���X�i�[���W�X�^68				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD69		(ARMPF_VIC_BASE + 0x514)		/* ���荞�݃A�h���X�i�[���W�X�^69				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD70		(ARMPF_VIC_BASE + 0x518)		/* ���荞�݃A�h���X�i�[���W�X�^70				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD71		(ARMPF_VIC_BASE + 0x51C)		/* ���荞�݃A�h���X�i�[���W�X�^71				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD72		(ARMPF_VIC_BASE + 0x520)		/* ���荞�݃A�h���X�i�[���W�X�^72				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD73		(ARMPF_VIC_BASE + 0x524)		/* ���荞�݃A�h���X�i�[���W�X�^73				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD74		(ARMPF_VIC_BASE + 0x528)		/* ���荞�݃A�h���X�i�[���W�X�^74				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD75		(ARMPF_VIC_BASE + 0x52C)		/* ���荞�݃A�h���X�i�[���W�X�^75				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD76		(ARMPF_VIC_BASE + 0x530)		/* ���荞�݃A�h���X�i�[���W�X�^76				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD77		(ARMPF_VIC_BASE + 0x534)		/* ���荞�݃A�h���X�i�[���W�X�^77				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD78		(ARMPF_VIC_BASE + 0x538)		/* ���荞�݃A�h���X�i�[���W�X�^78				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD79		(ARMPF_VIC_BASE + 0x53C)		/* ���荞�݃A�h���X�i�[���W�X�^79				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD80		(ARMPF_VIC_BASE + 0x540)		/* ���荞�݃A�h���X�i�[���W�X�^80				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD81		(ARMPF_VIC_BASE + 0x544)		/* ���荞�݃A�h���X�i�[���W�X�^81				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD82		(ARMPF_VIC_BASE + 0x548)		/* ���荞�݃A�h���X�i�[���W�X�^82				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD83		(ARMPF_VIC_BASE + 0x54C)		/* ���荞�݃A�h���X�i�[���W�X�^83				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD84		(ARMPF_VIC_BASE + 0x550)		/* ���荞�݃A�h���X�i�[���W�X�^84				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD85		(ARMPF_VIC_BASE + 0x554)		/* ���荞�݃A�h���X�i�[���W�X�^85				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD86		(ARMPF_VIC_BASE + 0x558)		/* ���荞�݃A�h���X�i�[���W�X�^86				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD87		(ARMPF_VIC_BASE + 0x55C)		/* ���荞�݃A�h���X�i�[���W�X�^87				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD88		(ARMPF_VIC_BASE + 0x560)		/* ���荞�݃A�h���X�i�[���W�X�^88				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD89		(ARMPF_VIC_BASE + 0x564)		/* ���荞�݃A�h���X�i�[���W�X�^89				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD90		(ARMPF_VIC_BASE + 0x568)		/* ���荞�݃A�h���X�i�[���W�X�^90				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD91		(ARMPF_VIC_BASE + 0x56C)		/* ���荞�݃A�h���X�i�[���W�X�^91				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD92		(ARMPF_VIC_BASE + 0x570)		/* ���荞�݃A�h���X�i�[���W�X�^92				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD93		(ARMPF_VIC_BASE + 0x574)		/* ���荞�݃A�h���X�i�[���W�X�^93				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD94		(ARMPF_VIC_BASE + 0x578)		/* ���荞�݃A�h���X�i�[���W�X�^94				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD95		(ARMPF_VIC_BASE + 0x57C)		/* ���荞�݃A�h���X�i�[���W�X�^95				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD96		(ARMPF_VIC_BASE + 0x580)		/* ���荞�݃A�h���X�i�[���W�X�^96				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD97		(ARMPF_VIC_BASE + 0x584)		/* ���荞�݃A�h���X�i�[���W�X�^97				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD98		(ARMPF_VIC_BASE + 0x588)		/* ���荞�݃A�h���X�i�[���W�X�^98				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD99		(ARMPF_VIC_BASE + 0x58C)		/* ���荞�݃A�h���X�i�[���W�X�^99				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD100	(ARMPF_VIC_BASE + 0x590)		/* ���荞�݃A�h���X�i�[���W�X�^100				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD101	(ARMPF_VIC_BASE + 0x594)		/* ���荞�݃A�h���X�i�[���W�X�^101				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD102	(ARMPF_VIC_BASE + 0x598)		/* ���荞�݃A�h���X�i�[���W�X�^102				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD103	(ARMPF_VIC_BASE + 0x59C)		/* ���荞�݃A�h���X�i�[���W�X�^103				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD104	(ARMPF_VIC_BASE + 0x5A0)		/* ���荞�݃A�h���X�i�[���W�X�^104				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD105	(ARMPF_VIC_BASE + 0x5A4)		/* ���荞�݃A�h���X�i�[���W�X�^105				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD106	(ARMPF_VIC_BASE + 0x5A8)		/* ���荞�݃A�h���X�i�[���W�X�^106				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD107	(ARMPF_VIC_BASE + 0x5AC)		/* ���荞�݃A�h���X�i�[���W�X�^107				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD108	(ARMPF_VIC_BASE + 0x5B0)		/* ���荞�݃A�h���X�i�[���W�X�^108				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD109	(ARMPF_VIC_BASE + 0x5B4)		/* ���荞�݃A�h���X�i�[���W�X�^109				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD110	(ARMPF_VIC_BASE + 0x5B8)		/* ���荞�݃A�h���X�i�[���W�X�^110				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD111	(ARMPF_VIC_BASE + 0x5BC)		/* ���荞�݃A�h���X�i�[���W�X�^111				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD112	(ARMPF_VIC_BASE + 0x5C0)		/* ���荞�݃A�h���X�i�[���W�X�^112				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD113	(ARMPF_VIC_BASE + 0x5C4)		/* ���荞�݃A�h���X�i�[���W�X�^113				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD114	(ARMPF_VIC_BASE + 0x5C8)		/* ���荞�݃A�h���X�i�[���W�X�^114				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD115	(ARMPF_VIC_BASE + 0x5CC)		/* ���荞�݃A�h���X�i�[���W�X�^115				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD116	(ARMPF_VIC_BASE + 0x5D0)		/* ���荞�݃A�h���X�i�[���W�X�^116				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD117	(ARMPF_VIC_BASE + 0x5D4)		/* ���荞�݃A�h���X�i�[���W�X�^117				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD118	(ARMPF_VIC_BASE + 0x5D8)		/* ���荞�݃A�h���X�i�[���W�X�^118				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD119	(ARMPF_VIC_BASE + 0x5DC)		/* ���荞�݃A�h���X�i�[���W�X�^119				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD120	(ARMPF_VIC_BASE + 0x5E0)		/* ���荞�݃A�h���X�i�[���W�X�^120				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD121	(ARMPF_VIC_BASE + 0x5E4)		/* ���荞�݃A�h���X�i�[���W�X�^121				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD122	(ARMPF_VIC_BASE + 0x5E8)		/* ���荞�݃A�h���X�i�[���W�X�^122				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD123	(ARMPF_VIC_BASE + 0x5EC)		/* ���荞�݃A�h���X�i�[���W�X�^123				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD124	(ARMPF_VIC_BASE + 0x5F0)		/* ���荞�݃A�h���X�i�[���W�X�^124				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD125	(ARMPF_VIC_BASE + 0x5F4)		/* ���荞�݃A�h���X�i�[���W�X�^125				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD126	(ARMPF_VIC_BASE + 0x5F8)		/* ���荞�݃A�h���X�i�[���W�X�^126				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_VAD127	(ARMPF_VIC_BASE + 0x5FC)		/* ���荞�݃A�h���X�i�[���W�X�^127				*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_PRL0		(ARMPF_VIC_BASE + 0x800)		/* ���荞�ݗD�惌�x���i�[���W�X�^0				*/
#define	ARMPF_VIC_PRL1		(ARMPF_VIC_BASE + 0x804)		/* ���荞�ݗD�惌�x���i�[���W�X�^1				*/
#define	ARMPF_VIC_PRL2		(ARMPF_VIC_BASE + 0x808)		/* ���荞�ݗD�惌�x���i�[���W�X�^2				*/
#define	ARMPF_VIC_PRL3		(ARMPF_VIC_BASE + 0x80C)		/* ���荞�ݗD�惌�x���i�[���W�X�^3				*/
#define	ARMPF_VIC_PRL4		(ARMPF_VIC_BASE + 0x810)		/* ���荞�ݗD�惌�x���i�[���W�X�^4				*/
#define	ARMPF_VIC_PRL5		(ARMPF_VIC_BASE + 0x814)		/* ���荞�ݗD�惌�x���i�[���W�X�^5				*/
#define	ARMPF_VIC_PRL6		(ARMPF_VIC_BASE + 0x818)		/* ���荞�ݗD�惌�x���i�[���W�X�^6				*/
#define	ARMPF_VIC_PRL7		(ARMPF_VIC_BASE + 0x81C)		/* ���荞�ݗD�惌�x���i�[���W�X�^7				*/
#define	ARMPF_VIC_PRL8		(ARMPF_VIC_BASE + 0x820)		/* ���荞�ݗD�惌�x���i�[���W�X�^8				*/
#define	ARMPF_VIC_PRL9		(ARMPF_VIC_BASE + 0x824)		/* ���荞�ݗD�惌�x���i�[���W�X�^9				*/
#define	ARMPF_VIC_PRL10		(ARMPF_VIC_BASE + 0x828)		/* ���荞�ݗD�惌�x���i�[���W�X�^10				*/
#define	ARMPF_VIC_PRL11		(ARMPF_VIC_BASE + 0x82C)		/* ���荞�ݗD�惌�x���i�[���W�X�^11				*/
#define	ARMPF_VIC_PRL12		(ARMPF_VIC_BASE + 0x830)		/* ���荞�ݗD�惌�x���i�[���W�X�^12				*/
#define	ARMPF_VIC_PRL13		(ARMPF_VIC_BASE + 0x834)		/* ���荞�ݗD�惌�x���i�[���W�X�^13				*/
#define	ARMPF_VIC_PRL14		(ARMPF_VIC_BASE + 0x838)		/* ���荞�ݗD�惌�x���i�[���W�X�^14				*/
#define	ARMPF_VIC_PRL15		(ARMPF_VIC_BASE + 0x83C)		/* ���荞�ݗD�惌�x���i�[���W�X�^15				*/
#define	ARMPF_VIC_PRL16		(ARMPF_VIC_BASE + 0x840)		/* ���荞�ݗD�惌�x���i�[���W�X�^16				*/
#define	ARMPF_VIC_PRL17		(ARMPF_VIC_BASE + 0x844)		/* ���荞�ݗD�惌�x���i�[���W�X�^17				*/
#define	ARMPF_VIC_PRL18		(ARMPF_VIC_BASE + 0x848)		/* ���荞�ݗD�惌�x���i�[���W�X�^18				*/
#define	ARMPF_VIC_PRL19		(ARMPF_VIC_BASE + 0x84C)		/* ���荞�ݗD�惌�x���i�[���W�X�^19				*/
#define	ARMPF_VIC_PRL20		(ARMPF_VIC_BASE + 0x850)		/* ���荞�ݗD�惌�x���i�[���W�X�^20				*/
#define	ARMPF_VIC_PRL21		(ARMPF_VIC_BASE + 0x854)		/* ���荞�ݗD�惌�x���i�[���W�X�^21				*/
#define	ARMPF_VIC_PRL22		(ARMPF_VIC_BASE + 0x858)		/* ���荞�ݗD�惌�x���i�[���W�X�^22				*/
#define	ARMPF_VIC_PRL23		(ARMPF_VIC_BASE + 0x85C)		/* ���荞�ݗD�惌�x���i�[���W�X�^23				*/
#define	ARMPF_VIC_PRL24		(ARMPF_VIC_BASE + 0x860)		/* ���荞�ݗD�惌�x���i�[���W�X�^24				*/
#define	ARMPF_VIC_PRL25		(ARMPF_VIC_BASE + 0x864)		/* ���荞�ݗD�惌�x���i�[���W�X�^25				*/
#define	ARMPF_VIC_PRL26		(ARMPF_VIC_BASE + 0x868)		/* ���荞�ݗD�惌�x���i�[���W�X�^26				*/
#define	ARMPF_VIC_PRL27		(ARMPF_VIC_BASE + 0x86C)		/* ���荞�ݗD�惌�x���i�[���W�X�^27				*/
#define	ARMPF_VIC_PRL28		(ARMPF_VIC_BASE + 0x870)		/* ���荞�ݗD�惌�x���i�[���W�X�^28				*/
#define	ARMPF_VIC_PRL29		(ARMPF_VIC_BASE + 0x874)		/* ���荞�ݗD�惌�x���i�[���W�X�^29				*/
#define	ARMPF_VIC_PRL30		(ARMPF_VIC_BASE + 0x878)		/* ���荞�ݗD�惌�x���i�[���W�X�^30				*/
#define	ARMPF_VIC_PRL31		(ARMPF_VIC_BASE + 0x87C)		/* ���荞�ݗD�惌�x���i�[���W�X�^31				*/
#define	ARMPF_VIC_PRL32		(ARMPF_VIC_BASE + 0x880)		/* ���荞�ݗD�惌�x���i�[���W�X�^32				*/
#define	ARMPF_VIC_PRL33		(ARMPF_VIC_BASE + 0x884)		/* ���荞�ݗD�惌�x���i�[���W�X�^33				*/
#define	ARMPF_VIC_PRL34		(ARMPF_VIC_BASE + 0x888)		/* ���荞�ݗD�惌�x���i�[���W�X�^34				*/
#define	ARMPF_VIC_PRL35		(ARMPF_VIC_BASE + 0x88C)		/* ���荞�ݗD�惌�x���i�[���W�X�^35				*/
#define	ARMPF_VIC_PRL36		(ARMPF_VIC_BASE + 0x890)		/* ���荞�ݗD�惌�x���i�[���W�X�^36				*/
#define	ARMPF_VIC_PRL37		(ARMPF_VIC_BASE + 0x894)		/* ���荞�ݗD�惌�x���i�[���W�X�^37				*/
#define	ARMPF_VIC_PRL38		(ARMPF_VIC_BASE + 0x898)		/* ���荞�ݗD�惌�x���i�[���W�X�^38				*/
#define	ARMPF_VIC_PRL39		(ARMPF_VIC_BASE + 0x89C)		/* ���荞�ݗD�惌�x���i�[���W�X�^39				*/
#define	ARMPF_VIC_PRL40		(ARMPF_VIC_BASE + 0x8A0)		/* ���荞�ݗD�惌�x���i�[���W�X�^40				*/
#define	ARMPF_VIC_PRL41		(ARMPF_VIC_BASE + 0x8A4)		/* ���荞�ݗD�惌�x���i�[���W�X�^41				*/
#define	ARMPF_VIC_PRL42		(ARMPF_VIC_BASE + 0x8A8)		/* ���荞�ݗD�惌�x���i�[���W�X�^42				*/
#define	ARMPF_VIC_PRL43		(ARMPF_VIC_BASE + 0x8AC)		/* ���荞�ݗD�惌�x���i�[���W�X�^43				*/
#define	ARMPF_VIC_PRL44		(ARMPF_VIC_BASE + 0x8B0)		/* ���荞�ݗD�惌�x���i�[���W�X�^44				*/
#define	ARMPF_VIC_PRL45		(ARMPF_VIC_BASE + 0x8B4)		/* ���荞�ݗD�惌�x���i�[���W�X�^45				*/
#define	ARMPF_VIC_PRL46		(ARMPF_VIC_BASE + 0x8B8)		/* ���荞�ݗD�惌�x���i�[���W�X�^46				*/
#define	ARMPF_VIC_PRL47		(ARMPF_VIC_BASE + 0x8BC)		/* ���荞�ݗD�惌�x���i�[���W�X�^47				*/
#define	ARMPF_VIC_PRL48		(ARMPF_VIC_BASE + 0x8C0)		/* ���荞�ݗD�惌�x���i�[���W�X�^48				*/
#define	ARMPF_VIC_PRL49		(ARMPF_VIC_BASE + 0x8C4)		/* ���荞�ݗD�惌�x���i�[���W�X�^49				*/
#define	ARMPF_VIC_PRL50		(ARMPF_VIC_BASE + 0x8C8)		/* ���荞�ݗD�惌�x���i�[���W�X�^50				*/
#define	ARMPF_VIC_PRL51		(ARMPF_VIC_BASE + 0x8CC)		/* ���荞�ݗD�惌�x���i�[���W�X�^51				*/
#define	ARMPF_VIC_PRL52		(ARMPF_VIC_BASE + 0x8D0)		/* ���荞�ݗD�惌�x���i�[���W�X�^52				*/
#define	ARMPF_VIC_PRL53		(ARMPF_VIC_BASE + 0x8D4)		/* ���荞�ݗD�惌�x���i�[���W�X�^53				*/
#define	ARMPF_VIC_PRL54		(ARMPF_VIC_BASE + 0x8D8)		/* ���荞�ݗD�惌�x���i�[���W�X�^54				*/
#define	ARMPF_VIC_PRL55		(ARMPF_VIC_BASE + 0x8DC)		/* ���荞�ݗD�惌�x���i�[���W�X�^55				*/
#define	ARMPF_VIC_PRL56		(ARMPF_VIC_BASE + 0x8E0)		/* ���荞�ݗD�惌�x���i�[���W�X�^56				*/
#define	ARMPF_VIC_PRL57		(ARMPF_VIC_BASE + 0x8E4)		/* ���荞�ݗD�惌�x���i�[���W�X�^57				*/
#define	ARMPF_VIC_PRL58		(ARMPF_VIC_BASE + 0x8E8)		/* ���荞�ݗD�惌�x���i�[���W�X�^58				*/
#define	ARMPF_VIC_PRL59		(ARMPF_VIC_BASE + 0x8EC)		/* ���荞�ݗD�惌�x���i�[���W�X�^59				*/
#define	ARMPF_VIC_PRL60		(ARMPF_VIC_BASE + 0x8F0)		/* ���荞�ݗD�惌�x���i�[���W�X�^60				*/
#define	ARMPF_VIC_PRL61		(ARMPF_VIC_BASE + 0x8F4)		/* ���荞�ݗD�惌�x���i�[���W�X�^61				*/
#define	ARMPF_VIC_PRL62		(ARMPF_VIC_BASE + 0x8F8)		/* ���荞�ݗD�惌�x���i�[���W�X�^62				*/
#define	ARMPF_VIC_PRL63		(ARMPF_VIC_BASE + 0x8FC)		/* ���荞�ݗD�惌�x���i�[���W�X�^63				*/
//#define	ARMPF_VIC_PRL64		(ARMPF_VIC_BASE + 0x900)		/* ���荞�ݗD�惌�x���i�[���W�X�^64				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL65		(ARMPF_VIC_BASE + 0x904)		/* ���荞�ݗD�惌�x���i�[���W�X�^65				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL66		(ARMPF_VIC_BASE + 0x908)		/* ���荞�ݗD�惌�x���i�[���W�X�^66				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL67		(ARMPF_VIC_BASE + 0x90C)		/* ���荞�ݗD�惌�x���i�[���W�X�^67				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL68		(ARMPF_VIC_BASE + 0x910)		/* ���荞�ݗD�惌�x���i�[���W�X�^68				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL69		(ARMPF_VIC_BASE + 0x914)		/* ���荞�ݗD�惌�x���i�[���W�X�^69				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL70		(ARMPF_VIC_BASE + 0x918)		/* ���荞�ݗD�惌�x���i�[���W�X�^70				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL71		(ARMPF_VIC_BASE + 0x91C)		/* ���荞�ݗD�惌�x���i�[���W�X�^71				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL72		(ARMPF_VIC_BASE + 0x920)		/* ���荞�ݗD�惌�x���i�[���W�X�^72				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL73		(ARMPF_VIC_BASE + 0x924)		/* ���荞�ݗD�惌�x���i�[���W�X�^73				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL74		(ARMPF_VIC_BASE + 0x928)		/* ���荞�ݗD�惌�x���i�[���W�X�^74				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL75		(ARMPF_VIC_BASE + 0x92C)		/* ���荞�ݗD�惌�x���i�[���W�X�^75				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL76		(ARMPF_VIC_BASE + 0x930)		/* ���荞�ݗD�惌�x���i�[���W�X�^76				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL77		(ARMPF_VIC_BASE + 0x934)		/* ���荞�ݗD�惌�x���i�[���W�X�^77				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL78		(ARMPF_VIC_BASE + 0x938)		/* ���荞�ݗD�惌�x���i�[���W�X�^78				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL79		(ARMPF_VIC_BASE + 0x93C)		/* ���荞�ݗD�惌�x���i�[���W�X�^79				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL80		(ARMPF_VIC_BASE + 0x940)		/* ���荞�ݗD�惌�x���i�[���W�X�^80				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL81		(ARMPF_VIC_BASE + 0x944)		/* ���荞�ݗD�惌�x���i�[���W�X�^81				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL82		(ARMPF_VIC_BASE + 0x948)		/* ���荞�ݗD�惌�x���i�[���W�X�^82				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL83		(ARMPF_VIC_BASE + 0x94C)		/* ���荞�ݗD�惌�x���i�[���W�X�^83				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL84		(ARMPF_VIC_BASE + 0x950)		/* ���荞�ݗD�惌�x���i�[���W�X�^84				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL85		(ARMPF_VIC_BASE + 0x954)		/* ���荞�ݗD�惌�x���i�[���W�X�^85				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL86		(ARMPF_VIC_BASE + 0x958)		/* ���荞�ݗD�惌�x���i�[���W�X�^86				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL87		(ARMPF_VIC_BASE + 0x95C)		/* ���荞�ݗD�惌�x���i�[���W�X�^87				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL88		(ARMPF_VIC_BASE + 0x960)		/* ���荞�ݗD�惌�x���i�[���W�X�^88				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL89		(ARMPF_VIC_BASE + 0x964)		/* ���荞�ݗD�惌�x���i�[���W�X�^89				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL90		(ARMPF_VIC_BASE + 0x968)		/* ���荞�ݗD�惌�x���i�[���W�X�^90				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL91		(ARMPF_VIC_BASE + 0x96C)		/* ���荞�ݗD�惌�x���i�[���W�X�^91				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL92		(ARMPF_VIC_BASE + 0x970)		/* ���荞�ݗD�惌�x���i�[���W�X�^92				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL93		(ARMPF_VIC_BASE + 0x974)		/* ���荞�ݗD�惌�x���i�[���W�X�^93				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL94		(ARMPF_VIC_BASE + 0x978)		/* ���荞�ݗD�惌�x���i�[���W�X�^94				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL95		(ARMPF_VIC_BASE + 0x97C)		/* ���荞�ݗD�惌�x���i�[���W�X�^95				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL96		(ARMPF_VIC_BASE + 0x980)		/* ���荞�ݗD�惌�x���i�[���W�X�^96				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL97		(ARMPF_VIC_BASE + 0x984)		/* ���荞�ݗD�惌�x���i�[���W�X�^97				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL98		(ARMPF_VIC_BASE + 0x988)		/* ���荞�ݗD�惌�x���i�[���W�X�^98				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL99		(ARMPF_VIC_BASE + 0x98C)		/* ���荞�ݗD�惌�x���i�[���W�X�^99				*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL100	(ARMPF_VIC_BASE + 0x990)		/* ���荞�ݗD�惌�x���i�[���W�X�^100			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL101	(ARMPF_VIC_BASE + 0x994)		/* ���荞�ݗD�惌�x���i�[���W�X�^101			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL102	(ARMPF_VIC_BASE + 0x998)		/* ���荞�ݗD�惌�x���i�[���W�X�^102			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL103	(ARMPF_VIC_BASE + 0x99C)		/* ���荞�ݗD�惌�x���i�[���W�X�^103			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL104	(ARMPF_VIC_BASE + 0x9A0)		/* ���荞�ݗD�惌�x���i�[���W�X�^104			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL105	(ARMPF_VIC_BASE + 0x9A4)		/* ���荞�ݗD�惌�x���i�[���W�X�^105			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL106	(ARMPF_VIC_BASE + 0x9A8)		/* ���荞�ݗD�惌�x���i�[���W�X�^106			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL107	(ARMPF_VIC_BASE + 0x9AC)		/* ���荞�ݗD�惌�x���i�[���W�X�^107			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL108	(ARMPF_VIC_BASE + 0x9B0)		/* ���荞�ݗD�惌�x���i�[���W�X�^108			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL109	(ARMPF_VIC_BASE + 0x9B4)		/* ���荞�ݗD�惌�x���i�[���W�X�^109			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL110	(ARMPF_VIC_BASE + 0x9B8)		/* ���荞�ݗD�惌�x���i�[���W�X�^110			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL111	(ARMPF_VIC_BASE + 0x9BC)		/* ���荞�ݗD�惌�x���i�[���W�X�^111			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL112	(ARMPF_VIC_BASE + 0x9C0)		/* ���荞�ݗD�惌�x���i�[���W�X�^112			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL113	(ARMPF_VIC_BASE + 0x9C4)		/* ���荞�ݗD�惌�x���i�[���W�X�^113			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL114	(ARMPF_VIC_BASE + 0x9C8)		/* ���荞�ݗD�惌�x���i�[���W�X�^114			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL115	(ARMPF_VIC_BASE + 0x9CC)		/* ���荞�ݗD�惌�x���i�[���W�X�^115			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL116	(ARMPF_VIC_BASE + 0x9D0)		/* ���荞�ݗD�惌�x���i�[���W�X�^116			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL117	(ARMPF_VIC_BASE + 0x9D4)		/* ���荞�ݗD�惌�x���i�[���W�X�^117			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL118	(ARMPF_VIC_BASE + 0x9D8)		/* ���荞�ݗD�惌�x���i�[���W�X�^118			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL119	(ARMPF_VIC_BASE + 0x9DC)		/* ���荞�ݗD�惌�x���i�[���W�X�^119			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL120	(ARMPF_VIC_BASE + 0x9E0)		/* ���荞�ݗD�惌�x���i�[���W�X�^120			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL121	(ARMPF_VIC_BASE + 0x9E4)		/* ���荞�ݗD�惌�x���i�[���W�X�^121			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL122	(ARMPF_VIC_BASE + 0x9E8)		/* ���荞�ݗD�惌�x���i�[���W�X�^122			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL123	(ARMPF_VIC_BASE + 0x9EC)		/* ���荞�ݗD�惌�x���i�[���W�X�^123			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL124	(ARMPF_VIC_BASE + 0x9F0)		/* ���荞�ݗD�惌�x���i�[���W�X�^124			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL125	(ARMPF_VIC_BASE + 0x9F4)		/* ���荞�ݗD�惌�x���i�[���W�X�^125			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL126	(ARMPF_VIC_BASE + 0x9F8)		/* ���荞�ݗD�惌�x���i�[���W�X�^126			*//* JL-086�g�p�s�� */
//#define	ARMPF_VIC_PRL127	(ARMPF_VIC_BASE + 0x9FC)		/* ���荞�ݗD�惌�x���i�[���W�X�^127			*//* JL-086�g�p�s�� */
#define	ARMPF_VIC_TCR		(ARMPF_VIC_BASE + 0xC00)		/* �e�X�g�E���[�h�I�����W�X�^					*/
#define	ARMPF_VIC_TICR		(ARMPF_VIC_BASE + 0xC04)		/* �e�X�g�E���[�h���荞�ݓ��͐��䃌�W�X�^		*/
#define	ARMPF_VIC_TACR		(ARMPF_VIC_BASE + 0xC08)		/* �e�X�g�E���[�h���荞�݃A�h���X���䃌�W�X�^	*/
#define	ARMPF_VIC_IOS		(ARMPF_VIC_BASE + 0xC0C)		/* ���荞�ݗv���X�e�[�^�X�E���W�X�^				*/
#define	ARMPF_VIC_VAOS		(ARMPF_VIC_BASE + 0xC10)		/* ���荞�ݗv���A�h���X�E�X�e�[�^�X�E���W�X�^	*/

#endif
/*-------end of file----------------------------------------------------------------------------------------*/
