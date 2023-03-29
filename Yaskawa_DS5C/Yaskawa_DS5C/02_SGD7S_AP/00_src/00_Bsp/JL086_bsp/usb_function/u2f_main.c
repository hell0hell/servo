/**
 *******************************************************************************
 * @file  u2f_main.c
 * @brief program main for usb function
 * @attention 
 * @note 
 * 
 * $Rev:: 338                        $
 * $Date:: 2012-02-20 21:33:53 +0900#$
 *******************************************************************************
 */
/*
 *------------------------------------------------------------------------------
 * Notice 
 *  This is a sample program. 
 *  Renesas Electronics assumes no responsibility for any losses incurred. 
 *
 * Copyright(C) 2011 Renesas Electronics Corporation 
 * Copyright(C) 2011 Renesas Micro Systems Co., Ltd. 
 *------------------------------------------------------------------------------
 */

/*============================================================================*/
/* I N C L U D E                                                              */
/*============================================================================*/
#include "Basedef.h"
#include "errcodes.h"
//#include "arm.h"				//@		/*<S008>*/
//#include "nbpfahb32vic128.h"	//@		/*<S008>*/
#include "ARMPF_USB_FUNC.h"
#include "usb_f.h"
#include "u2f_desc.h"

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
// --- Max Packet Size ---
#define BULK_MAXPACKET_SIZE_HS	 512
#define BULK_MAXPACKET_SIZE_FS	  64
#define INT_MAXPACKET_SIZE_HS	1024
#define INT_MAXPACKET_SIZE_FS	  64


/*===========================================================================*/
/* S T R U C T                                                               */
/*===========================================================================*/

/*============================================================================*/
/* V A R I A B L E                                                            */
/*============================================================================*/
volatile UINT32			f_connect = U2F_PLUG_OUT;
volatile UINT32			f_setup = 0;
volatile UINT32			f_ep2_out = 0;
volatile UINT32			f_ep1_dmaend = 0;
volatile UINT32			f_ep2_dmaend = 0;

/*============================================================================*/
/* S T A T I C  F U N C                                                       */
/*============================================================================*/
static void set_maxpacket ( void );
static void clr_flag ( void );

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/
/**
 *******************************************************************************
  @brief USB Function �h���C�o������
  @param  �Ȃ�
  @retval �Ȃ�
 *******************************************************************************
*/
void u2f_init( void )
{
	UINT32	state;

	clr_flag();												// �t���O�E�N���A

//	init_u2f_control();

	//=========================================
	// EPC,PLL ���Z�b�g����
	//=========================================
	ARMPF_EPCTR &= ~(D_SYS_EPC_RST | D_SYS_PLL_RST);				// ���Z�b�g����

	//=========================================
	// System Bus-EPC Bridge �����ݒ�
	//=========================================
	ARMPF_SYSSCTR   = D_SYS_WAIT_MODE;							// �X���[�u�̃E�G�C�g����w��
	ARMPF_SYSMCTR   = 0x00000034;									// DMA�]�����̃A�[�r�g���[�V�����F���E���h���r��
	ARMPF_SYSBINTEN = D_SYS_VBUS_INT;								// VBUS���荞�݋���

	//=========================================
	// EPC �����ݒ�
	//=========================================
	// --- PLL_LOCK�҂� ---
	while ((ARMPF_EPCTR & D_SYS_PLL_LOCK) == 0) {					// PLL_LOCK�҂�
		// !!!!!!!! ToDo �ُ펞���� !!!!!!!!
	}

	// --- ���荞�ݐݒ� ---
	ARMPF_USB_CONTROL   = D_USB_INT_SEL | D_USB_SOF_RCV;			// INTU2FEPC���荞�݂����x�����荞�݂ɐݒ�
															// SOF ��M�G���[�������Ɏ������J�o���@�\���g�p
	ARMPF_USB_INT_ENA   = D_USB_USB_RST_EN    |					// ���荞�݋��F�o�X�E���Z�b�g
					D_USB_SPEED_MODE_EN |					//             �F�o�X�E�X�s�[�h�ύX
					D_USB_EP0_EN        |					//             �FEP0 ���荞��
					D_USB_EP1_EN        |					//             �FEP1 ���荞��
					D_USB_EP2_EN;							//             �FEP2 ���荞��

	// --- Max Packet�ݒ� ---
	set_maxpacket();

	// --- �eEP �o�b�t�@�E�N���A ---
	ARMPF_EP0_CONTROL   = D_EP0_BCLR;								// EP0 : �o�b�t�@�E�N���A
//	ARMPF_EP1_CONTROL   = D_EPn_BCLR | D_EPn_AUTO;				// EP1 : Bulk-in,  �o�b�t�@�E�N���A, �����f�[�^���M�L��
	ARMPF_EP1_CONTROL   = D_EPn_BCLR;								// EP1 : Bulk-in,  �o�b�t�@�E�N���A
	ARMPF_EP2_CONTROL   = D_EPn_BCLR | D_EPn_DIR0;				// EP2 : Bulk-out, �o�b�t�@�E�N���A

	// --- �eEP ���荞�݋��� ---
	ARMPF_EP0_INT_ENA   = D_EP0_SETUP_EN;							// EP0 : ���荞�݋��FSETUP_INT
	ARMPF_EP1_INT_ENA   = D_EPn_IN_END_EN;						// EP1 : ���荞�݋��FEP1_IN_END_INT
	ARMPF_EP2_INT_ENA   = D_EPn_OUT_EN | D_EPn_OUT_END_EN;		// EP2 : ���荞�݋��FEP2_OUT_INT, EP2_OUT_END

	// --- �eEP �L���� ---
	ARMPF_EP1_CONTROL  |= D_EPn_EN;								// EP1 : �L����
	ARMPF_EP2_CONTROL  |= D_EPn_EN;								// EP2 : �L����

	//=========================================
	// �v���O�C���^�A�E�g���� (�N������Ԋm�F)
	//=========================================
	if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL)	// VBUS���x���m�F
	{
		// ====== �v���O�C������ ======
		state       =  ARMPF_USB_CONTROL;							//
		state      &= ~D_USB_CONNECTB;						// �iFunction PHY �ւ�USB �M����L���ɂ���j
		state      |=  D_USB_PUE2;							// �iD+�M����Pull-up ����j
		ARMPF_USB_CONTROL =  state;								// ��L����𓯎��ɔ��f
		f_connect = U2F_PLUG_IN;
	}
	else
	{
		// ====== �v���O�A�E�g���� ======
		state       =  ARMPF_USB_CONTROL;							//
		state      |=  D_USB_CONNECTB;						// �iFunction PHY �ւ�USB �M���𖳌��ɂ���j
		state      &= ~D_USB_PUE2;							// �iD+�M����Pull-up ���Ȃ��j
		ARMPF_USB_CONTROL =  state;								// ��L����𓯎��ɔ��f
		f_connect = U2F_PLUG_OUT;
	}

	//=========================================
	// ���荞�ݐݒ�
	//=========================================
//@	intc_set_handler( IDX_INTU2F, u2f_int_intu2f );			// ���荞�݃n���h���o�^
//@	intc_set_prl( IDX_INTU2F, PRIORITY_LVL0 );				// ���荞�ݗD�惌�x��	LEVEL0
//@	intc_clear_eiint( IDX_INTU2F );							// ���荞�݃N���A
//@	intc_enable_eiint( IDX_INTU2F );						// ���荞�݋���
//@
//@	intc_set_handler( IDX_INTU2FEPC, u2f_int_intu2fepc );	// ���荞�݃n���h���o�^
//@	intc_set_prl( IDX_INTU2FEPC, PRIORITY_LVL0 );			// ���荞�ݗD�惌�x��	LEVEL0
//@	intc_clear_eiint( IDX_INTU2FEPC );						// ���荞�݃N���A
//@	intc_enable_eiint( IDX_INTU2FEPC );						// ���荞�݋���
}


/**
 *******************************************************************************
  @brief INTU2F ���荞�ݏ���
  @param  �Ȃ�
  @retval �Ȃ�
 *******************************************************************************
*/
void u2f_int_intu2f ( void )
{
	UINT32	state;

	//=========================================
	// �v���O�C���^�A�E�g���� (VBUS)
	//=========================================
	if ((ARMPF_SYSBINT & D_SYS_VBUS_INT) == D_SYS_VBUS_INT) {
		ARMPF_SYSBINT |= D_SYS_VBUS_INT;								// ���荞�ݗv���N���A
		if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL) {	// VBUS���x���m�F
			// ====== �v���O�C������ ======
			state       =  ARMPF_USB_CONTROL;							//
			state      &= ~D_USB_CONNECTB;						// �iFunction PHY �ւ�USB �M����L���ɂ���j
			state      |=  D_USB_PUE2;							// �iD+�M����Pull-up ����j
			ARMPF_USB_CONTROL =  state;								// ��L����𓯎��ɔ��f
			f_connect = U2F_PLUG_IN;
		} else {												//
			// ====== �v���O�A�E�g���� ======
			state       =  ARMPF_USB_CONTROL;							//
			state      |=  D_USB_CONNECTB;						// �iFunction PHY �ւ�USB �M���𖳌��ɂ���j
			state      &= ~D_USB_PUE2;							// �iD+�M����Pull-up ���Ȃ��j
			state      &= ~D_USB_DEFAULT;						// �i�G���h�|�C���g0�𖳌��ɂ���j
			state      &= ~D_USB_CONF;							// �i�G���h�|�C���g0�ȊO�𖳌��ɂ���j
			ARMPF_USB_CONTROL =  state;								// ��L����𓯎��ɔ��f
			f_connect = U2F_PLUG_OUT;
			clr_flag();											// �t���O�E�N���A
		}
	}

//@	intc_clear_eiint( IDX_INTU2F );								// INTU2F �v���N���A

}

/**
 *******************************************************************************
  @brief INTU2FEPC ���荞�ݏ���
  @param  �Ȃ�
  @retval �Ȃ�
 *******************************************************************************
*/
void u2f_int_intu2fepc ( void )
{
	//=========================================
	// �o�X�E���Z�b�g ����
	//=========================================
	if ((ARMPF_USB_INT_STA & D_USB_USB_RST_INT) == D_USB_USB_RST_INT) {
		ARMPF_USB_INT_STA = ~D_USB_USB_RST_INT;						// ���荞�ݗv���N���A
		// --- Max Packet�ݒ� ---
		set_maxpacket();
		// --- EP �o�b�t�@�E�N���A ---
		ARMPF_EP0_CONTROL   |= D_EP0_BCLR;							// EP0 : �o�b�t�@�E�N���A
		ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							// EP1 : �o�b�t�@�E�N���A
		ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							// EP2 : �o�b�t�@�E�N���A
		// --- �t���O�E�N���A ---
		clr_flag();												// �t���O�E�N���A
	}
	//=========================================
	// USB�o�X�E�X�s�[�h�ύX ����
	//=========================================
	if ((ARMPF_USB_INT_STA & D_USB_SPEED_MODE_INT) == D_USB_SPEED_MODE_INT) {
		ARMPF_USB_INT_STA = ~D_USB_SPEED_MODE_INT;					// ���荞�ݗv���N���A
		// --- Max Packet�ݒ� ---
		set_maxpacket();
	}

	//=========================================
	// EP0 ����
	//=========================================
	// ---  SETUP�f�[�^��M ---
	if ((ARMPF_EP0_STATUS & D_EP0_SETUP_INT) == D_EP0_SETUP_INT) {
		ARMPF_EP0_STATUS = ~D_EP0_SETUP_INT;							// ���荞�ݗv���N���A
		f_setup = 1;
	}
	//=========================================
	// EP1 ����
	//=========================================
	// ---  Bulk-In DMA�]���I�� ---
	if ((ARMPF_EP1_STATUS & D_EPn_IN_END_INT) == D_EPn_IN_END_INT) {
		ARMPF_EP1_STATUS = ~D_EPn_IN_END_INT;						// ���荞�ݗv���N���A
		f_ep1_dmaend = 1;
	}
	//=========================================
	// EP2 ����
	//=========================================
	// ---  Bulk-Out �f�[�^��M ---
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_INT) == D_EPn_OUT_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_INT;							// ���荞�ݗv���N���A
		f_ep2_out++;
	}
	// ---  Bulk-Out DMA�]���I�� ---
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_END_INT) == D_EPn_OUT_END_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_END_INT;						// ���荞�ݗv���N���A
		f_ep2_dmaend = 1;
	}

//@	intc_clear_eiint( IDX_INTU2FEPC );							// INTU2FEPC �v���N���A
}

/**
 ******************************************************************************
  @brief EP0 ���M
  @param  pBuf      -- ���M�f�[�^�i�[�̈�
  @param  size      -- ���M�T�C�Y
  @retval 0�ȏ�̒l�̂Ƃ� -- ���M�����o�C�g��
  @retval ���̒l�̂Ƃ�    -- �G���[�����ɂ�鋭���I��
 ******************************************************************************
*/
INT u2f_write_EP0( VUCHAR* pBuf, INT size )
{
	INT	iSendLength		= size;				// ���M�f�[�^��
	INT	iRemainSize		= 0;				// �c��f�[�^��

	UINT32	TmpData;
	UINT32	temp;
	INT		i;

	// �c��f�[�^��
	iRemainSize = iSendLength;

	while (iRemainSize > 0) {
		//-----------------------------
		// �o�b�t�@�E�G���v�e�B�m�F
		//-----------------------------
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  �o�b�t�@����ɂȂ�܂ŃE�G�C�g
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (f_connect == U2F_PLUG_OUT) {
				return ER_SYS;
			}
		}

		//-----------------------------
		// �o�b�t�@��������
		//  (4byte�P��, Max:64byte)
		//-----------------------------
		for (i = 0; ((i < 64) && (iRemainSize >= 4)); i += 4, iRemainSize -= 4) {
			// --- �f�[�^���� ---
			TmpData  = (UINT32)*pBuf++;
			TmpData |= (UINT32)*pBuf++ <<  8;
			TmpData |= (UINT32)*pBuf++ << 16;
			TmpData |= (UINT32)*pBuf++ << 24;
			// --- �o�b�t�@�������� ---
			ARMPF_EP0_WRITE = TmpData;
		}
		// --- �f�[�^���M������ ---
		temp  = ARMPF_EP0_CONTROL;											// CONTROL���W�X�^ ���[�h
		temp |= D_EP0_DEND;												// ���M����

		//-----------------------------
		// �[���o�C�g(3byte�ȉ�)����
		//-----------------------------
		if ((i < 64) && (iRemainSize > 0) && (iRemainSize < 4)) {
			// --- �[���f�[�^���� ---
			TmpData = 0;
			for (i = 0; i < iRemainSize; i++) {
				TmpData |= (UINT32)*pBuf++ << (8*i);
			}
			// --- �o�b�t�@�������� ---
			ARMPF_EP0_WRITE = TmpData;
			// --- �f�[�^���M���� ---
			temp |= ( ((UINT32)iRemainSize) << 5 ) & ( D_EP0_DW );			// �[���o�C�g�̐ݒ�
			iRemainSize = 0;
		}
		//-----------------------------
		// �f�[�^���M����
		//-----------------------------
		ARMPF_EP0_CONTROL = temp;												// CONTROL���W�X�^ ���C�g
	}

	return size;
}


/**
 ******************************************************************************
  @brief EPn ��M (PIO�]��)
  @param  num       -- EP�ԍ�
  @param  pBuf      -- ��M�f�[�^�i�[�̈�
  @param  size      -- ��M�v���T�C�Y
  @retval 0�ȏ�̒l�̂Ƃ� -- ��M�����o�C�g��
  @retval ���̒l�̂Ƃ�    -- �G���[�����ɂ�鋭���I��
 ******************************************************************************
*/

INT u2f_read_EPn( UINT num, VUCHAR* pBuf, INT size )
{
	INT i;
	INT cnt;

	INT	iReadLength;							// ��M�f�[�^��
	INT	iRemainSize;							// �c��f�[�^��
	UINT32	TmpData;

	// �c��f�[�^��
	iRemainSize = size;
	cnt = 0;

	while (iRemainSize > 0) {
		// --- ��M�o�C�g���m�F ---
		iReadLength = *((VUINT32*)(EPn_LEN_DCNT_ADR + (num - 1) * 0x20)) & 0x3ff;

		// --- ��M�҂��^�C���A�E�g���f(��M�҂���Ԃ�SOF10��J�E���g���������Ƃ�) ---
		if (iReadLength == 0) {
			if ((ARMPF_USB_INT_STA & D_USB_SOF_INT) == D_USB_SOF_INT) {
				ARMPF_USB_INT_STA &= ~D_USB_SOF_INT;					// SOF��M���N���A
				if (cnt++ >= 10) {
					break;
				}
			}
		} else {
			cnt = 0;
		}

		// --- �f�[�^��M ---
		while (iReadLength > 0) {
			TmpData = *((VUINT32*)(EPn_READ_ADR + (num - 1) * 0x20));
			for (i = 0; ((i < 4) && (iReadLength > 0)); i++, iReadLength--, iRemainSize--) {
				*pBuf++ = (UINT8)(TmpData >> (8*i));
			}
		}
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}

//	__DI();
	f_ep2_out--;
//	__EI();

	return (size - iRemainSize);
}

/**
 ******************************************************************************
  @brief EPn ��M (DMA�]��)
  @param  num       -- EP�ԍ�
  @param  pBuf      -- ��M�f�[�^�i�[�̈�
  @param  size      -- ��M�T�C�Y
  @retval 0�ȏ�̒l�̂Ƃ� -- ���M�����o�C�g��
  @retval ���̒l�̂Ƃ�    -- �G���[�����ɂ�鋭���I��
 ******************************************************************************
*/

INT u2f_read_EPn_DMA( UINT num, VUCHAR* pBuf, INT size )
{
	UINT32	MaxPacketSize;
//	UINT32	LastPacketSize;
	UINT32	TransPacketNum;

	//=================================
	// �]���p�P�b�g���Z�o
	//=================================
	// --- EPn�̃}�b�N�X�E�p�P�b�g�E�T�C�Y�擾 ---
	MaxPacketSize  = (*((VUINT32*)(EPn_PCKT_ADRS_ADR + (num - 1) * 0x20)) & 0x0000ffff);	// EPn�̃}�b�N�X�E�p�P�b�g�E�T�C�Y�擾
	// --- �]���p�P�b�g���Z�o---
	TransPacketNum = size / MaxPacketSize;													// �]���p�P�b�g���Z�o
	// --- �ő�p�P�b�g������ & �ŏI�p�P�b�g�]���T�C�Y�Z�o ---
	if (TransPacketNum > 256) {																// �]���p�P�b�g����256���傫���Ƃ�(H/W����)�C
		// --- �ő�p�P�b�g���I�[�o ---
		TransPacketNum = 256;																//  -> �]���p�P�b�g����256�ɐ���
//		LastPacketSize = MaxPacketSize;														//  -> �ŏI�p�P�b�g�ł̓]���T�C�Y��0�ɐݒ�
	} else {																				// �]���p�P�b�g����256�ȉ��̂Ƃ��ŁC
		if ((TransPacketNum > 0) && ((size % MaxPacketSize) == 0)) {						// �t���E�p�P�b�g�ł���΁C
			// --- �t���E�p�P�b�g ---
//			LastPacketSize = MaxPacketSize;													//  -> �ŏI�p�P�b�g�E�T�C�Y���}�b�N�X�E�p�P�b�g�ɐݒ�
		} else {																			// �V���[�g�E�p�P�b�g�ł���΁C
			// --- �V���[�g�E�p�P�b�g ---
//			LastPacketSize = ((size % MaxPacketSize) & 0x7fc);								//  -> �ŏI�p�P�b�g�]���T�C�Y�Z�o
		}
	}

	//=================================
	// Bridge�� DMA�]�������ݒ�
	//=================================
	// --- �}�b�N�X�E�p�P�b�g�E�T�C�Y�ݒ� ---
	*((VUINT32*)(EPnDCR2_ADR + (num - 1) * 0x10)) = MaxPacketSize;
	// --- �]����A�h���X�ݒ� ---
	*((VUINT32*)(EPnTADR_ADR + (num - 1) * 0x10)) = (UINT32)pBuf;

	// --- �]������ �ݒ� ---
	*((VUINT32*)(EPnDCR1_ADR + (num - 1) * 0x10)) = D_SYS_EPn_DIR0;

	//=================================
	// EPC�� DMA�]�������ݒ�
	//=================================
	*((VUINT32*)(EPn_DMA_CTRL_ADR + (num - 1) * 0x20)) = (D_EPn_STOP_MODE | 0x00000101);

	//=================================
	// �f�[�^��M�҂�
	//=================================
	while (f_ep2_out == 0) {
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}
//	__DI();
	f_ep2_out--;
//	__EI();

	//=================================
	// Bridge�� DMA�]���J�n����
	//=================================
	// --- �]���p�P�b�g��, �]������, DMA�]������ �ݒ� ---
	//  �i�]���p�P�b�g����256�̂Ƃ��́C�]���p�P�b�g����0x00�Ɛݒ�j
	*((VUINT32*)(EPnDCR1_ADR + (num - 1) * 0x10)) = (((TransPacketNum & 0xff) << 16) | D_SYS_EPn_DIR0 | D_SYS_EPn_REQEN);

	//=================================
	// EPC�� DMA�]���J�n����
	//=================================
	// --- �]���p�P�b�g�� �ݒ� ---
	*((VUINT32*)(EPn_LEN_DCNT_ADR + (num - 1) * 0x20)) = (TransPacketNum << 16);
	// EPC �o�[�X�g�ݒ� & DMA�]������
	*((VUINT32*)(EPn_DMA_CTRL_ADR + (num - 1) * 0x20)) |= (D_EPn_BURST_SET | D_EPn_DMA_EN);

	//=================================
	// DMA�]���I���҂�
	//=================================
	while ((f_ep2_dmaend & 1) != 1) {
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}
	f_ep2_dmaend = 0;
//	__DI();
	f_ep2_out = 0;
//	__EI();

	return size;
}


/**
 ******************************************************************************
  @brief EPn ���M (PIO�]��)
  @param  num       -- EP�ԍ�
  @param  pBuf      -- ���M�f�[�^�i�[�̈�
  @param  size      -- ���M�T�C�Y
  @retval 0�ȏ�̒l�̂Ƃ� -- ���M�����o�C�g��
  @retval ���̒l�̂Ƃ�    -- �G���[�����ɂ�鋭���I��
 ******************************************************************************
*/

INT u2f_write_EPn( UINT num, VUCHAR* pBuf, INT size )
{
	INT	iRemainSize;						// �c��f�[�^��

	UINT32	MaxPacketSize;
	UINT32	TmpData;
	UINT32	temp;
	INT		i;

	// �c��f�[�^��
	iRemainSize = size;

	// --- �����f�[�^���M���� ---
	*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) &= ~D_EPn_AUTO;

	// --- EPn�̃}�b�N�X�E�p�P�b�g�E�T�C�Y�擾 ---
	MaxPacketSize  = (*((VUINT32*)(EPn_PCKT_ADRS_ADR + (num - 1) * 0x20)) & 0x0000ffff);	// EPn�̃}�b�N�X�E�p�P�b�g�E�T�C�Y�擾

	while (iRemainSize > 0) {
		//-----------------------------
		// �o�b�t�@�E�G���v�e�B�m�F
		//-----------------------------
		while((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) {	//  �o�b�t�@����ɂȂ�܂ŃE�G�C�g
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (f_connect == U2F_PLUG_OUT) {
				return ER_SYS;
			}
		}

		//-----------------------------
		// �o�b�t�@��������
		//  (4byte�P��, Max:Max Packet Size)
		//-----------------------------
		for (i = 0; ((i < MaxPacketSize) && (iRemainSize >= 4)); i += 4, iRemainSize -= 4) {
			// --- �f�[�^���� ---
			TmpData  = (UINT32)*pBuf++;
			TmpData |= (UINT32)*pBuf++ <<  8;
			TmpData |= (UINT32)*pBuf++ << 16;
			TmpData |= (UINT32)*pBuf++ << 24;
			// --- �o�b�t�@�������� ---
			*((VUINT32*)(EPn_WRITE_ADR + (num - 1) * 0x20)) = TmpData;
		}
		// --- �f�[�^���M������ ---
		temp  = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));											// CONTROL���W�X�^ ���[�h
		temp |= D_EPn_DEND;												// ���M����

		//-----------------------------
		// �[���o�C�g(3byte�ȉ�)����
		//-----------------------------
		if ((i < MaxPacketSize) && (iRemainSize > 0) && (iRemainSize < 4)) {
			// --- �[���f�[�^���� ---
			TmpData = 0;
			for (i = 0; i < iRemainSize; i++) {
				TmpData |= (UINT32)*pBuf++ << (8*i);
			}
			// --- �o�b�t�@�������� ---
			*((VUINT32*)(EPn_WRITE_ADR + (num - 1) * 0x20)) = TmpData;
			// --- �f�[�^���M���� ---
			temp |= ( ((UINT32)iRemainSize) << 5 ) & ( D_EP0_DW );			// �[���o�C�g�̐ݒ�
			iRemainSize = 0;
		}
		//-----------------------------
		// �f�[�^���M����
		//-----------------------------
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;										// CONTROL���W�X�^ ���C�g
	}

	return size;
}

/**
 ******************************************************************************
  @brief EPn ���M (DMA�]��)
  @param  num       -- EP�ԍ�
  @param  pBuf      -- ���M�f�[�^�i�[�̈�
  @param  size      -- ���M�T�C�Y
  @retval 0�ȏ�̒l�̂Ƃ� -- ���M�����o�C�g��
  @retval ���̒l�̂Ƃ�    -- �G���[�����ɂ�鋭���I��
 ******************************************************************************
*/

INT u2f_write_EPn_DMA( UINT num, VUCHAR* pBuf, INT size )
{
	UINT32	MaxPacketSize, LastPacketSize;
	UINT32	TransPacketNum;

	// --- �����f�[�^���M�L�� ---
	*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_AUTO;

	//=================================
	// �]���p�P�b�g���Z�o
	//=================================
	// --- EPn�̃}�b�N�X�E�p�P�b�g�E�T�C�Y�擾 ---
	MaxPacketSize  = (*((VUINT32*)(EPn_PCKT_ADRS_ADR + (num - 1) * 0x20)) & 0x0000ffff);	// EPn�̃}�b�N�X�E�p�P�b�g�E�T�C�Y�擾
	// --- �]���p�P�b�g���Z�o---
	TransPacketNum = size / MaxPacketSize;													// �]���p�P�b�g���Z�o
	// --- �ő�p�P�b�g������ & �ŏI�p�P�b�g�]���T�C�Y�Z�o ---
	if (TransPacketNum > 256)
	{																// �]���p�P�b�g����256���傫���Ƃ�(H/W����)�C
		// --- �ő�p�P�b�g���I�[�o ---
		TransPacketNum = 256;																//  -> �]���p�P�b�g����256�ɐ���
		LastPacketSize = MaxPacketSize;														//  -> �ŏI�p�P�b�g�ł̓]���T�C�Y��0�ɐݒ�
	}
	else
	{																				// �]���p�P�b�g����256�ȉ��̂Ƃ��ŁC
		if ((TransPacketNum > 0) && ((size % MaxPacketSize) == 0))
		{// �t���E�p�P�b�g�ł���΁C
			// --- �t���E�p�P�b�g ---
			LastPacketSize = MaxPacketSize;													//  -> �ŏI�p�P�b�g�E�T�C�Y���}�b�N�X�E�p�P�b�g�ɐݒ�
		}
		else
		{// �V���[�g�E�p�P�b�g�ł���΁C
			// --- �V���[�g�E�p�P�b�g ---
			LastPacketSize = ((size % MaxPacketSize) & 0x7fc);								//  -> �ŏI�p�P�b�g�]���T�C�Y�Z�o
		}
	}

	//=================================
	// Bridge�� DMA�]�������ݒ�
	//=================================
	// --- �}�b�N�X�E�p�P�b�g & �ŏI�p�P�b�g�]���T�C�Y�ݒ� ---
	*((VUINT32*)(EPnDCR2_ADR + (num - 1) * 0x10)) = ((LastPacketSize << 16) | MaxPacketSize);
	// --- �]����A�h���X�ݒ� ---
	*((VUINT32*)(EPnTADR_ADR + (num - 1) * 0x10)) = (UINT32)pBuf;

	// --- �]���p�P�b�g��, �]������, DMA�]������ �ݒ� ---
	//  �i�]���p�P�b�g����256�̂Ƃ��́C�]���p�P�b�g����0x00�Ɛݒ�j
	*((VUINT32*)(EPnDCR1_ADR + (num - 1) * 0x10)) = (((TransPacketNum & 0xff) << 16) | D_SYS_EPn_REQEN);

	//=================================
	// EPC�� DMA�]�������ݒ�i�]���J�n�j
	//=================================
	// --- �]���p�P�b�g�� �ݒ� ---
	*((VUINT32*)(EPn_LEN_DCNT_ADR + (num - 1) * 0x20)) = (TransPacketNum << 16);
	// EPC �o�[�X�g�ݒ� & DMA�]������
	*((VUINT32*)(EPn_DMA_CTRL_ADR + (num - 1) * 0x20)) = (D_EPn_BURST_SET | D_EPn_DMA_EN | 0x00000101);

	//=================================
	// DMA�]���I���҂�
	//=================================
	while ((f_ep1_dmaend & 1) != 1)
	{
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if
		(f_connect == U2F_PLUG_OUT)
		{
			return ER_SYS;
		}
	}
	f_ep1_dmaend = 0;

	return size;
}

/**
 ******************************************************************************
  @brief EP0 �X�g�[������
  @param  �Ȃ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_set_stall_EP0( void )
{
	ARMPF_EP0_CONTROL |= D_EP0_STL;
}

/**
 ******************************************************************************
  @brief EPn �X�g�[������
  @param  num       -- EP�ԍ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_set_stall_EPn( UINT num )
{
	// --- IN/OUT ���f ---
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		// --- BULK IN ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_ISTL;
	} else {
		// --- BULK OUT ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= (D_EPn_OSTL_EN | D_EPn_OSTL);
	}
}

/**
 ******************************************************************************
  @brief EPn �X�g�[������
  @param  num       -- EP�ԍ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_clear_stall_EPn( UINT num )
{
	UINT32	temp;

	// --- IN/OUT ���f ---
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		// --- BULK IN ---
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp &= ~(D_EPn_ISTL);											// EPn : IN�g�[�N�� STALL����
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

	} else {
		// --- BULK OUT ---
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_OSTL_EN;											// EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���
		temp &= ~(D_EPn_OSTL);											// EPn : OUT/PING�g�[�N�� STALL����
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
	}
}

/**
 ******************************************************************************
  @brief EPn �o�b�t�@�E�N���A
  @param  num       -- EP�ԍ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_clear_buf_EPn( UINT num )
{
	*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_BCLR;			// EPn : �o�b�t�@�E�N���A
}

/**
 ******************************************************************************
  @brief EPn PID�N���A
  @param  num       -- EP�ԍ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_clear_PID_EPn( UINT num )
{
	// --- IN/OUT ���f ---
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		// --- BULK IN ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_IPIDCLR;		// EPn : ���MPID�N���A
	} else {
		// --- BULK OUT ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_OPIDCLR;		// EPn : ��MPID�N���A
	}
}

/**
 ******************************************************************************
  @brief EP0 NAK����
  @param  �Ȃ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_set_nak_EP0( void )
{
	UINT32 temp;

	temp  = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp |= (UINT32)(D_EP0_INAK | D_EP0_ONAK);
	ARMPF_EP0_CONTROL = temp;
}

/**
 ******************************************************************************
  @brief EP0 NAK����
  @param  �Ȃ�
  @retval �Ȃ�
 ******************************************************************************
*/

void u2f_clear_nak_EP0( void )
{
	UINT32 temp;

	temp  = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)(D_EP0_INAK | D_EP0_ONAK);
	ARMPF_EP0_CONTROL = temp;
}


/**
 ******************************************************************************
  @brief EP0 NULL���M
  @param  �Ȃ�
  @retval �Ȃ�
 ******************************************************************************
*/

ER_RET u2f_send_null_EP0( void )
{
	//-----------------------------
	// �o�b�t�@�E�G���v�e�B�m�F
	//-----------------------------
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  �o�b�t�@����ɂȂ�܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}

	//-----------------------------
	// �f�[�^���M����
	//-----------------------------
	ARMPF_EP0_CONTROL |= D_EP0_DEND;									// ���M����

	return ER_OK;
}

/**
 ******************************************************************************
  @brief USB�ڑ���Ԋm�F
  @param  �Ȃ�
  @retval U2F_PLUG_OUT -- USB Host�Ɛڑ�����Ă��Ȃ��Ƃ�
  @retval U2F_PLUG_IN  -- USB Host�Ɛڑ�����Ă���Ƃ�
 ******************************************************************************
*/

UINT u2f_check_plug_inout( void )
{
	return f_connect;
}

/**
 ******************************************************************************
  @brief Max Packet Size �ݒ�
  @param  �Ȃ�
  @retval �Ȃ�
 ******************************************************************************
*/

static void set_maxpacket ( void )
{
	UINT32	bulk_size, interrupt_size;

	//---------------------------------
	// USB�o�X�E�X�s�[�h�m�F
	//---------------------------------
	if ( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE ) {
		// High Speed
		bulk_size      = BULK_MAXPACKET_SIZE_HS;
		interrupt_size = INT_MAXPACKET_SIZE_HS;
	} else {
		// Full Speed
		bulk_size      = BULK_MAXPACKET_SIZE_FS;
		interrupt_size = INT_MAXPACKET_SIZE_FS;
	}

	//---------------------------------
	// Max Packet Size �ݒ�
	//---------------------------------
														// EPn : BaseAdr  MaxPkt(HS/FS �{�h���C�o�ł͋K�i��̍ő�l��ݒ�)
														// EP0 : 0x0000        64 byte x 2��
	ARMPF_EP1_PCKT_ADRS = 0x00200000 | bulk_size;				// EP1 : 0x0020    512/64 byte x 2��	--   Bulk    �]�� --
	ARMPF_EP2_PCKT_ADRS = 0x01200000 | bulk_size;				// EP2 : 0x0120    512/64 byte x 2��	--   Bulk    �]�� --
	ARMPF_EP3_PCKT_ADRS = 0x02200000 | interrupt_size;		// EP3 : 0x0220   1024/64 byte x 1��	-- Interrupt �]�� --
}

/**
 ******************************************************************************
  @brief �t���O�E�N���A
  @param  �Ȃ�
  @retval �Ȃ�
 ******************************************************************************
*/
static void clr_flag ( void )
{
	f_setup = 0;
	f_ep2_out = 0;
	f_ep1_dmaend = 0;
	f_ep2_dmaend = 0;
}
