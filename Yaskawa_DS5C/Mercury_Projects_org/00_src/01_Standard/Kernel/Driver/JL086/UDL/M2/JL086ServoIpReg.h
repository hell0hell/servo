/****************************************************************************************************/
/*																									*/
/*																									*/
/*		JL076Reg.h : JL-076B/077B���W�X�^��`�w�b�_�t�@�C��											*/
/*			���t�@�C������JL086�����A���g��JL076��													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. JL-076B/077B���W�X�^��`																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2012.08.06 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef _JL076REG_H // �ēǂݍ��ݖh�~�p



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-076B/077B H/W���W�X�^�\���̒�`														*/
/*																									*/
/****************************************************************************************************/
typedef struct ASIC_HW_IF_STR {
	VUSHORT	AREG_PWMU;				/* 0x0000 �A�i���O���j�^PWM�o��U								*/
	VUSHORT	AREG_PWMV;				/* 0x0002 �A�i���O���j�^PWM�o��V								*/
	VUSHORT	AREG_IOSTAT1;			/* 0x0004 �ėp�����V���A���o��1�����M�o�b�t�@�]��				*/
	VUSHORT	AREG_IOSTAT2;			/* 0x0006 �ėp�����V���A���o��2									*/
	VUSHORT	AREG_ITDO;				/* 0x0008 ���������p�^�C�}(RD)									*/
	VUSHORT	AREG_TIM1SET;			/* 0x000A �^�C�}1�ݒ背�W�X�^									*/
	VUSHORT	AREG_UARTSTS;			/* 0x000C UART�X�e�[�^�X										*/
	VUSHORT	AREG_UARTTXD;			/* 0x000E �ėp�񓯊��|�[�g���M�f�[�^(WR)����M�f�[�^(RD)		*/
	VUSHORT	AREG_DIVCSEV;			/* 0x0010 �G���R�[�_�p���X�o�͕�����ݒ�						*/
	VUSHORT	AREG_EPRSET;			/* 0x0012 EEPROM�FIF�ݒ�										*/
	VUSHORT	AREG_EPRLW;				/* 0x0014 EEPROM�F���M�f�[�^���ʃ��[�h							*/
	VUSHORT	AREG_EPRHW;				/* 0x0016 EEPROM�F���M�f�[�^��ʃ��[�h							*/
	VUSHORT	AREG_EPRCNT;			/* 0x0018 EEPROM�F�N���b�N���ݒ聕���M�J�n						*/
	VUSHORT	AREG_INT1SET;			/* 0x001A INT1�����ݐݒ�&�A�N�m���b�W							*/
	VUSHORT	AREG_PFNCSET;			/* 0x001C �[�q�@�\�ݒ�											*/
	VUSHORT	SPARE0;					/* 0x001E Reserve												*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_FBCSET;			/* 0x0020 �������̓t�B���^�ݒ�									*/
	VUSHORT	AREG_POCSET;			/* 0x0022 �w�߃J�E���^�ݒ�										*/
	VUSHORT	AREG_DINCSET;			/* 0x0024 �����@�\�ݒ�											*/
	VUSHORT	AREG_LODSET;			/* 0x0026 ���[�h�ݒ�											*/
	VUSHORT	AREG_T_FBLD;			/* 0x0028 FB�J�E���^�ݒ�										*/
	VUSHORT	AREG_T_CMDLD;			/* 0x002A CMD�J�E���^�ݒ�										*/
	VUSHORT	AREG_ALMCLR;			/* 0x002C �A���[�����b�`���Z�b�g								*/
	VUSHORT	AREG_ALMSET;			/* 0x002E �A���[���ݒ�											*/
	VUSHORT	AREG_IOSTATX;			/* 0x0030 �ėp����												*/
	VUSHORT	AREG_FLTSTAT;			/* 0x0032 �̏����												*/
	VUSHORT	SPARE1;					/* 0x0034 Reserve												*/
	VUSHORT	AREG_SETREG1;			/* 0x0036 �����ݒ背�W�X�^										*/
	VUSHORT	AREG_SETREG2;			/* 0x0038 �@�\�I�����W�X�^										*/
	VUSHORT	AREG_CLKO;				/* 0x003A �V���A���G���R�[�_IF�`�����x�ݒ背�W�X�^				*/
	VUSHORT	AREG_CSTP;				/* 0x003C �@�\��~�I�����W�X�^("1"�Œ�~)						*/
	VUSHORT	AREG_SEPGSET00;			/* 0x003E �V���A���G���R�[�_IF�ݒ�FCH0�ݒ�0					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_SETREG3;			/* 0x0040 �񐶋@�\�ݒ背�W�X�^									*/
	VUSHORT	AREG_SEPGSET01;			/* 0x0042 �V���A���G���R�[�_IF�ݒ�FCH0�ݒ�1					*/
	VUSHORT	AREG_SEPGSET10;			/* 0x0044 �V���A���G���R�[�_IF�ݒ�FCH1�ݒ�0					*/
	VUSHORT	AREG_SEPGSET11;			/* 0x0046 �V���A���G���R�[�_IF�ݒ�FCH1�ݒ�1					*/
	VUSHORT	AREG_PIOSET;			/* 0x0048 �g��I/O�ݒ背�W�X�^									*/
	VUSHORT	AREG_FCCSET;			/* 0x004A �w�߃t�B���^�ݒ背�W�X�^								*/
	VUSHORT	AREG_T_FLC;				/* 0x004C �t���N���[�Y�h�J�E���^�ݒ�							*/
	VUSHORT	AREG_ESYNC;				/* 0x004E �V���A���G���R�[�_�����J�E���^�O������				*/
	VUSHORT	AREG_SE0_TXD0;			/* 0x0050 �V���A���G���R�[�_CH0���M�f�[�^0						*/
	VUSHORT	AREG_SE0_TXD1;			/* 0x0052 �V���A���G���R�[�_CH0���M�f�[�^1						*/
	VUSHORT	AREG_SE0_TXD2;			/* 0x0054 �V���A���G���R�[�_CH0���M�f�[�^2						*/
	VUSHORT	AREG_SE0_TXD3;			/* 0x0056 �V���A���G���R�[�_CH0���M�f�[�^3						*/
	VUSHORT	AREG_SE0_TXD4;			/* 0x0058 �V���A���G���R�[�_CH0���M�f�[�^4						*/
	VUSHORT	AREG_SE0_TXD5;			/* 0x005A �V���A���G���R�[�_CH0���M�f�[�^5						*/
	VUSHORT	AREG_SE0_TXD6;			/* 0x005C �V���A���G���R�[�_CH0���M�f�[�^6						*/
	VUSHORT	AREG_SE0_TXD7;			/* 0x005E �V���A���G���R�[�_CH0���M�f�[�^7						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_SE0_RXD0;			/* 0x0060 �V���A���G���R�[�_CH0��M�f�[�^0						*/
	VUSHORT	AREG_SE0_RXD1;			/* 0x0062 �V���A���G���R�[�_CH0��M�f�[�^1						*/
	VUSHORT	AREG_SE0_RXD2;			/* 0x0064 �V���A���G���R�[�_CH0��M�f�[�^2						*/
	VUSHORT	AREG_SE0_RXD3;			/* 0x0066 �V���A���G���R�[�_CH0��M�f�[�^3						*/
	VUSHORT	AREG_SE0_RXD4;			/* 0x0068 �V���A���G���R�[�_CH0��M�f�[�^4						*/
	VUSHORT	AREG_SE0_RXD5;			/* 0x006A �V���A���G���R�[�_CH0��M�f�[�^5						*/
	VUSHORT	AREG_SE0_RXD6;			/* 0x006C �V���A���G���R�[�_CH0��M�f�[�^6						*/
	VUSHORT	AREG_SE0_RXD7;			/* 0x006E �V���A���G���R�[�_CH0��M�f�[�^7						*/
	VUSHORT	AREG_GSSRDL;			/* 0x0070 �ėp�����V���A����M�f�[�^L							*/
	VUSHORT	AREG_GSSRDH;			/* 0x0072 �ėp�����V���A����M�f�[�^H							*/
	VUSHORT	AREG_CMDCLML;			/* 0x0074 �w�߃J�E���^CML���b�`�l								*/
	VUSHORT	AREG_SPRG0CR;			/* 0x0076 SENC�FCH0�R�}���h/���X�|���X�o�b�t�@					*/
	VUSHORT	AREG_CDLTD;				/* 0x0078 �V���A���G���R�[�_CH0�J�E���^���b�`�l					*/
	VUSHORT	AREG_CMDLDL;			/* 0x007A �w�߃J�E���^���[�h���b�`�l							*/
	VUSHORT	AREG_SPRG1CR;			/* 0x007C SENC�FCH1�R�}���h/���X�|���X�o�b�t�@					*/
	VUSHORT	AREG_SEDLLD;			/* 0x007E �V���A���G���R�[�_�x���������[�h�l					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_ADCTL;				/* 0x0080 �ėpADIF����											*/
	VUSHORT	SPARE2;					/* 0x0082 Reserve												*/
	VUSHORT	AREG_TSTEELWL;			/* 0x0084 EEPROM IF Low���[�h�l(�e�X�g�p)						*/
	VUSHORT	AREG_TSTDCCNT;			/* 0x0086 ����DDA���ݒl											*/
	VUSHORT	AREG_TSTT1CNT;			/* 0x0088 �^�C�}T1�l(�e�X�g�p)									*/
	VUSHORT	SPARE3;					/* 0x008A Reserve												*/
	VUSHORT	AREG_TSTPWMUCNT;		/* 0x008C PWMU�J�E���^����8�r�b�g�l(�e�X�g�p)					*/
	VUSHORT	AREG_TSTPWMVCNT;		/* 0x008E PWMV�J�E���^����8�r�b�g�l(�e�X�g�p)					*/
	VUSHORT	AREG_SDMCONF1;			/* 0x0090 �f�V���[�V�����t�B���^�ݒ�							*/
	VUSHORT	AREG_SDMCONF2;			/* 0x0092 �I�[�o�����W���o�}�X�N�r�b�g�ݒ�A�����ݒ�			*/
	VUSHORT	AREG_PWMCCD;			/* 0x0094 PWM�L�����A�J�E���^���ݒl								*/
	VUSHORT	SPARE4;					/* 0x0096 Reserve												*/
	VUSHORT	AREG_DYSETUP1;			/* 0x0098 U��P���I���f�B���C���Ԑݒ�1							*/
	VUSHORT	AREG_DYSETUN1;			/* 0x009A U��N���I���f�B���C���Ԑݒ�1							*/
	VUSHORT	AREG_DYSETVP1;			/* 0x009C V��P���I���f�B���C���Ԑݒ�1							*/
	VUSHORT	AREG_DYSETVN1;			/* 0x009E V��N���I���f�B���C���Ԑݒ�1							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_DYSETWP1;			/* 0x00A0 W��P���I���f�B���C���Ԑݒ�1							*/
	VUSHORT	AREG_DYSETWN1;			/* 0x00A2 W��N���I���f�B���C���Ԑݒ�1							*/
	VUSHORT	SPARE5[2];				/* 0x00A4-0x00A6 Reserve										*/
	VUSHORT	AREG_ZDR;				/* 0x00A8 �p���X�ϊ����_�␳�ݒ�								*/
	VUSHORT	AREG_ZENBR;				/* 0x00AA ���_�ʉ߃r�b�g�ʒu�ݒ�								*/
	VUSHORT	AREG_DIVHSR;			/* 0x00AC ������H�q�X�e���V�X�ݒ�								*/
	VUSHORT	SPARE6;					/* 0x00AE Reserve												*/
	VUSHORT	AREG_PUMPON;			/* 0x00B0 �`���[�W�|���v�ݒ�									*/
	VUSHORT	AREG_DIVSET;			/* 0x00B2 �����@�\�ݒ�											*/
	VUSHORT	AREG_PLSET;				/* 0x00B4 �p���X�ϊ��ݒ�										*/
	VUSHORT	AREG_WDT1SET;			/* 0x00B6 ���Z���pWDT�ݒ�										*/
	VUSHORT	AREG_POSET0;			/* 0x00B8 �V���A��PG-�p���X�ϊ��ʒu�ݒ�							*/
	VUSHORT	AREG_POSET1;			/* 0x00BA �V���A��PG-�p���X�ϊ����_�␳1						*/
	VUSHORT	AREG_POSET2;			/* 0x00BC �V���A��PG-�p���X�ϊ����_�␳2						*/
	VUSHORT	AREG_POTEST;			/* 0x00BE �V���A��PG-�p���X�ϊ��ݒ�L(�e�X�g�p)					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_POSET0H;			/* 0x00C0 �V���A��PG-�p���X�ϊ��ʒu�ݒ�bit16-23					*/
	VUSHORT	AREG_PGDCAL;			/* 0x00C2 �V���A��PG-�p���X�ϊ��V�t�g��,�f�[�^���ݒ�			*/
	VUSHORT	AREG_POSET12H;			/* 0x00C4 �V���A��PG-�p���X�ϊ����_�␳H						*/
	VUSHORT	AREG_WDT2SET;			/* 0x00C6 �z�X�g�pWDT�ݒ�										*/
	VUSHORT	AREG_DCTRIM;			/* 0x00C8 �����o�͌��_�␳�ݒ�(Bit0-15)							*/
	VUSHORT	AREG_ADSET1;			/* 0x00CA AD�ݒ�												*/
	VUSHORT	AREG_ADSET2;			/* 0x00CC �w��AD�ݒ�F��{�N���b�N								*/
	VUSHORT	AREG_ADSET3;			/* 0x00CE �w��AD�ݒ�FIF�T�C�N��								*/
	VUSHORT	AREG_ADSET4;			/* 0x00D0 �w��AD�ݒ�F���b�`�^�C�~���O							*/
	VUSHORT	AREG_ADSET5;			/* 0x00D2 �w��AD�ݒ�F�R���g���[��CH0							*/
	VUSHORT	AREG_ADSET6;			/* 0x00D4 �w��AD�ݒ�F�R���g���[��CH1							*/
	VUSHORT	AREG_ADSYNC;			/* 0x00D6 �w��AD�T�C�N������									*/
	VUSHORT	AREG_ADFCNT1;			/* 0x00D8 �d��AD�ݒ�F��{�N���b�N								*/
	VUSHORT	AREG_ADFCNT2;			/* 0x00DA �d��AD�ݒ�FIF�T�C�N��								*/
	VUSHORT	AREG_ADFCNT3;			/* 0x00DC �d��AD�ݒ�F���b�`�^�C�~���O							*/
	VUSHORT	AREG_ADFCNT4;			/* 0x00DE �d��AD�ݒ�F�R���g���[��CH0							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_ADFCNT5;			/* 0x00E0 �d��AD�ݒ�F�R���g���[��CH1							*/
	VUSHORT	AREG_ADFSYNC;			/* 0x00E2 �d��AD�T�C�N������									*/
	VUSHORT	AREG_INT0SET;			/* 0x00E4 INT0�����ݐݒ�										*/
	VUSHORT	AREG_ACULH;				/* 0x00E6 �w��AD�FCH0���b�`�f�[�^(Bit16-23)						*/
	VUSHORT	AREG_INT0TRG;			/* 0x00E8 INT0�����ݓ����|�[�g/�J�E���^�l�Ǐo��					*/
	VUSHORT	AREG_DCNTV;				/* 0x00EA �����J�E���^�ݒ�l(Bit0-15)							*/
	VUSHORT	AREG_ACUL;				/* 0x00EC �w��AD�FCH0���b�`�f�[�^								*/
	VUSHORT	AREG_ACVLL;				/* 0x00EE �w��AD�FCH1���b�`�f�[�^(Bit0-15)						*/
	VUSHORT	AREG_AFUL;				/* 0x00F0 �d��AD�FU���d�����b�`�f�[�^							*/
	VUSHORT	AREG_AFVL;				/* 0x00F2 �d��AD�FV���d�����b�`�f�[�^							*/
	VUSHORT	AREG_CMDAD1;			/* 0x00F4 �w��AD�f�[�^1(�e�X�g�p)								*/
	VUSHORT	AREG_CMDAD2;			/* 0x00F6 �w��AD�f�[�^2(�e�X�g�p)								*/
	VUSHORT	AREG_CURAD1;			/* 0x00F8 �d��AD�f�[�^1(�e�X�g�p)								*/
	VUSHORT	AREG_CURAD2;			/* 0x00FA �d��AD�f�[�^2(�e�X�g�p)								*/
	VUSHORT	AREG_DDACT;				/* 0x00FC �V���A��PG-�p���X�ϊ�DDA�l(�e�X�g�p)					*/
	VUSHORT	AREG_ACVLH;				/* 0x00FE �w��AD�FCH1���b�`�f�[�^(Bit16-23)						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_MAD0;				/* 0x0100 �ėpAD�`���l��0�f�[�^									*/
	VUSHORT	AREG_MAD1;				/* 0x0102 �ėpAD�`���l��1�f�[�^									*/
	VUSHORT	AREG_MAD2;				/* 0x0104 �ėpAD�`���l��2�f�[�^									*/
	VUSHORT	AREG_MAD3;				/* 0x0106 �ėpAD�`���l��3�f�[�^									*/
	VUSHORT	AREG_MAD4;				/* 0x0108 �ėpAD�`���l��4�f�[�^									*/
	VUSHORT	AREG_MAD5;				/* 0x010A �ėpAD�`���l��5�f�[�^									*/
	VUSHORT	AREG_MAD6;				/* 0x010C �ėpAD�`���l��6�f�[�^									*/
	VUSHORT	AREG_MAD7;				/* 0x010E �ėpAD�`���l��7�f�[�^									*/
	VUSHORT	AREG_MADCTL;			/* 0x0110 �ėpADIF�ݒ�											*/
	VUSHORT	AREG_MADT12;			/* 0x0112 �ėpADIF�`�b�v�Z���N�g�^�C�~���O�ݒ�					*/
	VUSHORT	AREG_MADT34;			/* 0x0114 �ėpADIF�N���b�N�^�C�~���O�ݒ�						*/
	VUSHORT	AREG_MADT56;			/* 0x0116 �ėpADIF�}���`�v���N�T�C�l�[�u���ݒ�					*/
	VUSHORT	AREG_MUMSK;				/* 0x0118 �ėpADIF�}���`�v���N�T�`���l���}�X�N�ݒ�				*/
	VUSHORT	AREG_I2CCNT;			/* 0x011A I2C EEPROM �|�[�g�R���g���[��							*/
	VUSHORT	AREG_I2CDATA;			/* 0x011C I2C EEPROM ���M�f�[�^									*/
	VUSHORT	AREG_I2CDI;				/* 0x011E I2C EEPROM ��M�f�[�^									*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_NCTCSET;			/* 0x0120 �w�ߗpNCT�ݒ�											*/
	VUSHORT	AREG_AFULH;				/* 0x0122 �d��AD�FCH0���b�`�f�[�^(Bit16-23)						*/
	VUSHORT	AREG_AFVLH;				/* 0x0124 �d��AD�FCH1���b�`�f�[�^(Bit16-23)						*/
	VUSHORT	AREG_ACMC4;				/* 0x0126 �w�߃}���`�v���N�T�`���l��4							*/
	VUSHORT	AREG_NCTCTM;			/* 0x0128 �w�ߗpNCT�F�^�C�}�J�E���^�l							*/
	VUSHORT	AREG_NCTCFB;			/* 0x012A �w�ߗpNCT�FFB�J�E���^�l								*/
	VUSHORT	AREG_NCTCCD;			/* 0x012C �w�ߗpNCT�FCD�J�E���^�l								*/
	VUSHORT	AREG_NCTCTP;			/* 0x012E �w�ߗpNCT�FTP�J�E���^�l								*/
	VUSHORT	AREG_SP_130;			/* 0x0130 �V���A��PG-�p���X�ϊ��ݒ�H(�e�X�g�p)					*/
	VUSHORT	AREG_HINT1;				/* 0x0132 ���Z�������݃|�[�g1									*/
	VUSHORT	AREG_HINT2;				/* 0x0134 ���Z�������݃|�[�g2									*/
	VUSHORT	AREG_ACMC5;				/* 0x0136 �w�߃}���`�v���N�T�`���l��5							*/
	VUSHORT	AREG_SRESET;			/* 0x0138 �\�t�g���Z�b�g										*/
	VUSHORT	AREG_NCTRSTL;			/* 0x013A NCT���Z�b�g											*/
	VUSHORT	AREG_ACMC6;				/* 0x013C �w�߃}���`�v���N�T�`���l��6							*/
	VUSHORT	AREG_ACMC7;				/* 0x013E �w�߃}���`�v���N�T�`���l��7							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_SE1_TXD0;			/* 0x0140 �V���A���G���R�[�_CH1���M�f�[�^0						*/
	VUSHORT	AREG_SE1_TXD1;			/* 0x0142 �V���A���G���R�[�_CH1���M�f�[�^1						*/
	VUSHORT	AREG_SE1_TXD2;			/* 0x0144 �V���A���G���R�[�_CH1���M�f�[�^2						*/
	VUSHORT	AREG_SE1_TXD3;			/* 0x0146 �V���A���G���R�[�_CH1���M�f�[�^3						*/
	VUSHORT	AREG_SE1_TXD4;			/* 0x0148 �V���A���G���R�[�_CH1���M�f�[�^4						*/
	VUSHORT	AREG_SE1_TXD5;			/* 0x014A �V���A���G���R�[�_CH1���M�f�[�^5						*/
	VUSHORT	AREG_SE1_TXD6;			/* 0x014C �V���A���G���R�[�_CH1���M�f�[�^6						*/
	VUSHORT	AREG_SE1_TXD7;			/* 0x014E �V���A���G���R�[�_CH1���M�f�[�^7						*/
	VUSHORT	AREG_SE1_RXD0;			/* 0x0150 �V���A���G���R�[�_CH1��M�f�[�^0						*/
	VUSHORT	AREG_SE1_RXD1;			/* 0x0152 �V���A���G���R�[�_CH1��M�f�[�^1						*/
	VUSHORT	AREG_SE1_RXD2;			/* 0x0154 �V���A���G���R�[�_CH1��M�f�[�^2						*/
	VUSHORT	AREG_SE1_RXD3;			/* 0x0156 �V���A���G���R�[�_CH1��M�f�[�^3						*/
	VUSHORT	AREG_SE1_RXD4;			/* 0x0158 �V���A���G���R�[�_CH1��M�f�[�^4						*/
	VUSHORT	AREG_SE1_RXD5;			/* 0x015A �V���A���G���R�[�_CH1��M�f�[�^5						*/
	VUSHORT	AREG_SE1_RXD6;			/* 0x015C �V���A���G���R�[�_CH1��M�f�[�^6						*/
	VUSHORT	AREG_SE1_RXD7;			/* 0x015E �V���A���G���R�[�_CH1��M�f�[�^7						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_STIMLT0;			/* 0x0160 ���b�`�^�C�}���b�`�l0									*/
	VUSHORT	AREG_STIMLT1;			/* 0x0162 ���b�`�^�C�}���b�`�l1									*/
	VUSHORT	AREG_STIMLT2;			/* 0x0164 ���b�`�^�C�}���b�`�l2									*/
	VUSHORT	AREG_STIMLT3;			/* 0x0166 ���b�`�^�C�}���b�`�l3									*/
	VUSHORT	AREG_STIMLT4;			/* 0x0168 ���b�`�^�C�}���b�`�l4									*/
	VUSHORT	AREG_STIMSET;			/* 0x016A ���b�`�^�C�}�ݒ�										*/
	VUSHORT	AREG_HPDO;				/* 0x016C �g��I/O�o�̓f�[�^										*/
	VUSHORT	AREG_HPDI;				/* 0x016E �g��I/O���̓f�[�^										*/
	VUSHORT	AREG_SADAT_A;			/* 0x0170 �G���R�[�_AD�f�[�^A									*/
	VUSHORT	AREG_SADAT_AA;			/* 0x0172 �G���R�[�_AD�f�[�^AA									*/
	VUSHORT	AREG_SADAT_B;			/* 0x0174 �G���R�[�_AD�f�[�^B									*/
	VUSHORT	AREG_SADAT_BB;			/* 0x0176 �G���R�[�_AD�f�[�^BB									*/
	VUSHORT	AREG_EIFSET1;			/* 0x0178 �G���R�[�_ADIF�ݒ�1									*/
	VUSHORT	AREG_EIFSET2;			/* 0x017A �G���R�[�_ADIF�ݒ�2									*/
	VUSHORT	AREG_SRCNT0;			/* 0x017C �V���A���G���R�[�_IF0�`�������ݒ�						*/
	VUSHORT	AREG_SRCNT1;			/* 0x017E �V���A���G���R�[�_IF1�`�������ݒ�						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_FBZRND;			/* 0x0180 �t�B�[�h�o�b�N�J�E���^ZRN���b�`�f�[�^					*/
	VUSHORT	AREG_FBCLT;				/* 0x0182 �t�B�[�h�o�b�N�J�E���^���[�h���b�`�f�[�^				*/
	VUSHORT	AREG_FBPUD;				/* 0x0184 �t�B�[�h�o�b�N�J�E���^PU���b�`�f�[�^					*/
	VUSHORT	AREG_FBPCD;				/* 0x0186 �t�B�[�h�o�b�N�J�E���^PC���b�`�f�[�^					*/
	VUSHORT	AREG_TCNT;				/* 0x0188 �t�B�[�h�o�b�N�^�C�}���ݒl							*/
	VUSHORT	AREG_T1LT;				/* 0x018A �t�B�[�h�o�b�N�^�C�}A/B�p���X���b�`�l					*/
	VUSHORT	AREG_T2LT;				/* 0x018C �t�B�[�h�o�b�N�^�C�}�L�����A���b�`�l					*/
	VUSHORT	AREG_T3LT;				/* 0x018E �t�B�[�h�o�b�N�^�C�}T1���O�l���b�`�l					*/
	VUSHORT	AREG_FCCFZRNL;			/* 0x0190 �t���N���[�Y�h�J�E���^ZRN���b�`�f�[�^					*/
	VUSHORT	AREG_FCLDL;				/* 0x0192 �t���N���[�Y�h�J�E���^���[�h���b�`�f�[�^				*/
	VUSHORT	AREG_DCTRIMH;			/* 0x0194 �����o�͌��_�␳�ݒ�(Bit16-23)						*/
	VUSHORT	AREG_DCNTVH;			/* 0x0196 �����J�E���^�ݒ�l(Bit16-23)							*/
	VUSHORT	AREG_MPTCY;				/* 0x0198 M-Port�X���[�u�^�C�}�����ݒ�							*/
	VUSHORT	AREG_M_TXS;				/* 0x019A �X���[�u���M�v���I��/�����ݗv��						*/
	VUSHORT	AREG_M_COM;				/* 0x019C M-Port�ʐM����										*/
	VUSHORT	AREG_M_STS;				/* 0x019E M-Port�ʐM�X�e�[�^�X									*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	SPARE7[48];				/* 0x01A0-0x01FE Reserve										*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_UPLDA;				/* 0x0200 ASIC MicroPrg Load Pointer							*/
	VUSHORT	MREG_UPLD;				/* 0x0202 ASIC MicroPrg Load Buffer								*/
	VUSHORT	MREG_CTSTW;				/* 0x0204 ����status(READ-ONLY)									*/
	VUSHORT	MREG_CTSTR;				/* 0x0206 ����cmdport(WRITE-ONLY)								*/
	VUSHORT	MREG_HSUR;				/* 0x0208 �z�X�g�w�߃|�[�g(WRITE-ONLY)							*/
}ASIC_HW_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-076B/077B �ʃv���O�������W�X�^�\���̒�`											*/
/*																									*/
/****************************************************************************************************/
typedef struct MICRO_CYC_OUTPUTS {
	VUSHORT	MREG_BITCMDI;			/* 0x7D40 Bit Command from Host									*/
	VUSHORT	MREG_CTRLMDI;			/* 0x7D42 Control Mode from Host								*/
	VUSHORT	MREG_ZCTRMDI;			/* 0x7D44 Zero Control Mode from Host							*/
	VUSHORT	MREG_DCVOLTA;			/* 0x7D46 ��ߊE���p�����[�^�v�Z�pPN�d�����ϒl					*/
	VUSHORT	MREG_TLMTP24IN;			/* 0x7D48 �����g���N�����l [2^24/MaxTrq] Lo						*/
	VUSHORT	MREG_TLMTP24INH;		/* 0x7D4A �����g���N�����l [2^24/MaxTrq] Hi						*/
	VUSHORT	MREG_TLMTM24IN;			/* 0x7D4C �����g���N�����l [2^24/MaxTrq] Lo						*/
	VUSHORT	MREG_TLMTM24INH;		/* 0x7D4E �����g���N�����l [2^24/MaxTrq] Hi						*/
	VUSHORT	MREG_DPOSREFI;			/* 0x7D50 dPosition Reference from Host Lo						*/
	VUSHORT	MREG_DPOSREFIH;			/* 0x7D52 dPosition Reference from Host Hi						*/
	VUSHORT	MREG_SPDREFI;			/* 0x7D54 Speed Reference from Host Lo							*/
	VUSHORT	MREG_SPDREFIH;			/* 0x7D56 Speed Reference from Host Hi							*/
	VUSHORT	MREG_TRQREFI;			/* 0x7D58 Torque Reference from Host(2^24) Lo					*/
	VUSHORT	MREG_TRQREFIH;			/* 0x7D5A Torque Reference from Host(2^24) Hi					*/
	VUSHORT	MREG_TRQTBL;			/* 0x7D5C �e�[�u���^�]/�O���g���N�w�� Lo						*/
	VUSHORT	MREG_TRQTBLH;			/* 0x7D5E �e�[�u���^�]/�O���g���N�w�� Hi						*/
	VUSHORT	MREG_SPDFBI;			/* 0x7D60 ���xFB�⏞���� Lo										*/
	VUSHORT	MREG_SPDFBIH;			/* 0x7D62 ���xFB�⏞���� Hi										*/
	VUSHORT	MREG_KVVIN;				/* 0x7D64 �d���␳�Q�C��										*/
	VUSHORT	MREG_Z_IQDISTIN;		/* 0x7D66 q���O���g���N���� (for dummy write)					*/
} MICRO_CYC_OUTPUTS;
/*--------------------------------------------------------------------------------------------------*/

typedef struct MICRO_CYC_INPUTS {
	VUSHORT	MREG_BITDAT;			/* 0x7D80 Bit Data												*/
	VUSHORT	MREG_RXALMCODE;			/* 0x7D82 Alarm Code											*/
	VUSHORT	MREG_CPHDIST;			/* 0x7D84 Motor Position from C-Phase Lo						*/
	VUSHORT	MREG_CPHDISTH;			/* 0x7D86 Motor Position from C-Phase Hi						*/
	VUSHORT	MREG_MOTPOSERR;			/* 0x7D88 Position Error(32bits) Lo								*/
	VUSHORT	MREG_MOTPOSERRH;		/* 0x7D8A Position Error(32bits) Hi								*/
	VUSHORT	MREG_MONSPDFFC;			/* 0x7D8C Speed Feedforward Compensation Lo						*/
	VUSHORT	MREG_MONSPDFFCH;		/* 0x7D8E Speed Feedforward Compensation Hi						*/
	VUSHORT	MREG_TRQREFOA;			/* 0x7D90 Torque Reference Output (Average) Lo					*/
	VUSHORT	MREG_TRQREFOAH;			/* 0x7D92 Torque Reference Output (Average) Hi					*/
	VUSHORT	MREG_TRQFILOUT;			/* 0x7D94 Torque Filter Output Lo								*/
	VUSHORT	MREG_TRQFILOUTH;		/* 0x7D96 Torque Filter Output Hi								*/
	VUSHORT	MREG_MONSPDREF;			/* 0x7D98 Speed Reference (Position Loop Output) Lo				*/
	VUSHORT	MREG_MONSPDREFH;		/* 0x7D9A Speed Reference (Position Loop Output) Hi				*/
	VUSHORT	MREG_RSPMOTPOS;			/* 0x7D9C �����^�C�~���O��MOTPOS�쐬�p(LONG) Lo					*/
	VUSHORT	MREG_RSPMOTPOSH;		/* 0x7D9E �����^�C�~���O��MOTPOS�쐬�p(LONG) Hi					*/
	VUSHORT	MREG_MOTSPDA;			/* 0x7DA0 Average Motor Speed Lo  [2^24/OvrSpd]					*/
	VUSHORT	MREG_MOTSPDAH;			/* 0x7DA2 Average Motor Speed Hi  [2^24/OvrSpd]					*/
	VUSHORT	MREG_IDO;				/* 0x7DA4 d���d���w��											*/
	VUSHORT	MREG_IQMON;				/* 0x7DA6 IQREF�̃��j�^�p										*/
	VUSHORT	MREG_IQMONA;			/* 0x7DA8 IQREF(���ϒl)�̃��j�^�p								*/
	VUSHORT	MREG_CTSTRW;			/* 0x7DAA ����t���O/�X�e�[�^�X									*/
} MICRO_CYC_INPUTS;
/*--------------------------------------------------------------------------------------------------*/

typedef struct ASIC_U_IF_STR {
	VUSHORT	MREG_ZEROR;				/* 0x7000 ; Zero register(Low) */
	VUSHORT	MREG_ZERORH;			/* 0x7002 ; Zero register(High) */
	VUSHORT	MREG_IDIL;				/* 0x7004 ; ACRd Integral(Low) */
	VUSHORT	MREG_IDIH;				/* 0x7006 ; ACRd Integral(High) */
	VUSHORT	MREG_IQIL;				/* 0x7008 ; ACRq Integral(Low) */
	VUSHORT	MREG_IQIH;				/* 0x700A ; ACRq Integral(High) */
	VUSHORT	MREG_VDFL;				/* 0x700C ; ACRd Filter Output(Low) */
	VUSHORT	MREG_VDFH;				/* 0x700E ; ACRd Filter Output(High) */
	VUSHORT	MREG_VQFL;				/* 0x7010 ; ACRq Filter Output(Low) */
	VUSHORT	MREG_VQFH;				/* 0x7012 ; ACRq Filter Output(High) */
	VUSHORT	MREG_PHAL;				/* 0x7014 ; Phase(Low) */
	VUSHORT	MREG_PHA;				/* 0x7016 ; Phase(High) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_BBSETW;			/* 0x7018 ; soft BB set(BBSET) */
	VUSHORT	MREG_FLTINW;			/* 0x701A ; fault status(FLTIN) */
	VUSHORT	MREG_CTSTRW;			/* 0x701C ; controll flag/status(CTSTR,CTSTW) */
	VUSHORT	MREG_IUOF;				/* 0x701E ; A/D transfer offset for iu */
	VUSHORT	MREG_IVOF;				/* 0x7020 ; A/D transfer offset for iv */
	VUSHORT	MREG_KCU;				/* 0x7022 ; A/D transfer gain for iu */
	VUSHORT	MREG_KCV;				/* 0x7024 ; A/D transfer gain for iv */
	VUSHORT	MREG_IDO;				/* 0x7026 ; Id reference */
	VUSHORT	MREG_IQO;				/* 0x7028 ; Iq reference */
	VUSHORT	MREG_VEL;				/* 0x702A ; Velocity (omega) */
	VUSHORT	MREG_CNTSTS;			/* 0x702C ; counter status(FBCSTS) */
	VUSHORT	MREG_FCCSTMON;			/* 0x702E ; full closed counter status(FCCST) */
/*--------------------------------------------------------------------------------------------------*/
/*		counter																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_NCNT;				/* 0x7030 ; FB count */
	VUSHORT	MREG_DNCNT;				/* 0x7032 ; dNCNT */
/*--------------------------------------------------------------------------------------------------*/
/*		voltage FF																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_LDC;				/* 0x7034 ; Ld  * omega */
	VUSHORT	MREG_LQC;				/* 0x7036 ; Lq  * omega */
	VUSHORT	MREG_MAGC;				/* 0x7038 ; Phi * omega */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_SINT;				/* 0x703A ; sin theta */
	VUSHORT	MREG_COST;				/* 0x703C ; cos theta */
	VUSHORT	MREG_SINT2;				/* 0x703E ; sin(theta + 2pi/3) */
	VUSHORT	MREG_COST2;				/* 0x7040 ; cos(theta + 2pi/3) */
	VUSHORT	MREG_SINT3;				/* 0x7042 ; sin(theta - 2pi/3) */
	VUSHORT	MREG_COST3;				/* 0x7044 ; cos(theta - 2pi/3) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IUD;				/* 0x7046 ; iu input data (from A/D) */
	VUSHORT	MREG_IVD;				/* 0x7048 ; iv input data (from A/D) */
	VUSHORT	MREG_IDD;				/* 0x704A ; Id Input */
	VUSHORT	MREG_IQD;				/* 0x704C ; Iq Input */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VD;				/* 0x704E ; Vd output voltage */
	VUSHORT	MREG_VQ;				/* 0x7050 ; Vq output voltage */
	VUSHORT	MREG_VDC;				/* 0x7052 ; Vd compensation data */
	VUSHORT	MREG_VQC;				/* 0x7054 ; Vq compensation data */
	VUSHORT	MREG_VU;				/* 0x7056 ; vu output data (to PWM) */
	VUSHORT	MREG_VV;				/* 0x7058 ; vv output data (to PWM) */
	VUSHORT	MREG_VW;				/* 0x705A ; vw output data (to PWM) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy1;			/* 0x705C ; Dummy1 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_CRFQW;				/* 0x705E ; Carrier freq Now */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IUO;				/* 0x7060 ; Iu reference(for dead-time cmp) */
	VUSHORT	MREG_IVO;				/* 0x7062 ; Iv reference(for dead-time cmp) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_V1;				/* 0x7064 ; V1( = SQRT(VD^2+VQ^2) ) */
	VUSHORT	MREG_VC;				/* 0x7066 ; Central voltage */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VDREF;				/* 0x7068 ; vdref */
	VUSHORT	MREG_VQREF;				/* 0x706A ; vqref */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy2[10];		/* 0x706C - 0x707E */
/*--------------------------------------------------------------------------------------------------*/
/*		for LPF																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO1FL;			/* 0x7080 ; LPF1 Output(Low) */
	VUSHORT	MREG_IQO1F;				/* 0x7082 ; LPF1 Output(High) */
	VUSHORT	MREG_IQOFL;				/* 0x7084 ; LPF2 Output(Low) */
	VUSHORT	MREG_IQOF;				/* 0x7086 ; LPF2 Output(High) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQREF;				/* 0x7088 ; Iq Reference after limit(5/23) */
	VUSHORT	MREG_TLMTP;				/* 0x708A ; +tlim 5/23	 */
	VUSHORT	MREG_TLMTM;				/* 0x708C ; -tlim 5/23 */
/*--------------------------------------------------------------------------------------------------*/
/*		for monitor																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQMON;				/* 0x708E ; IQ monitor */
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2 work(output previous) 													*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQOP;				/* 0x7090 ; Filter2 output previous(Low) */
	VUSHORT	MREG_IQOPH;				/* 0x7092 ; Filter2 output previous(High) */
/*--------------------------------------------------------------------------------------------------*/
/*		for over modulation																			*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KM;				/* 0x7094 ; Over modulation gain/offset */
	VUSHORT	MREG_KVV;				/* 0x7096 ; AVR */
/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_ADRST;				/* 0x7098 ;  */
/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_ADERRCNT;			/* 0x709A ; ASIC AD Error Counter */
	VUSHORT	MREG_IRIUP;				/* 0x709C ; Previous IRIU */
	VUSHORT	MREG_IRIUCNT;			/* 0x709E ; same IRIU counter */
	VUSHORT	MREG_IRIVP;				/* 0x70A0 ; Previous IRIV */
	VUSHORT	MREG_IRIVCNT;			/* 0x70A2 ; same IRIV counter */
/*--------------------------------------------------------------------------------------------------*/
/*		reserved																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG82;			/* 0x70A4 ; Work  Register 82 */
	VUSHORT	MREG_WREG83;			/* 0x70A6 ; Work  Register 83 */
	VUSHORT	MREG_WREG84;			/* 0x70A8 ; Work  Register 84 */
	VUSHORT	MREG_WREG85;			/* 0x70AA ; Work  Register 85 */
	VUSHORT	MREG_WREG86;			/* 0x70AC ; Work  Register 86 */
	VUSHORT	MREG_WREG87;			/* 0x70AE ; Work  Register 87 */
	VUSHORT	MREG_WREG88;			/* 0x70B0 ; Work  Register 88 */
	VUSHORT	MREG_WREG89;			/* 0x70B2 ; Work  Register 89 */
/*--------------------------------------------------------------------------------------------------*/
/*		deferential																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG90;			/* 0x70B4 ; Work  Register 90 */
//	VDDFL	
//	IDDP	
	VUSHORT	MREG_WREG91;			/* 0x70B6 ; Work  Register 91 */
//	VDDFH	
	VUSHORT	MREG_WREG92;			/* 0x70B8 ; Work  Register 92 */
//	VQDFL	
//	IQDP	
	VUSHORT	MREG_WREG93;			/* 0x70BA ; Work  Register 93 */
//	VQDFH	
	VUSHORT	MREG_KEANGLE;			/* 0x70BC ; �@�B�p���d�C�p�ϊ��W�� */
	VUSHORT	MREG_WREG95;			/* 0x70BE ; Work  Register 95 */
	VUSHORT	MREG_IDFL;				/* 0x70C0 ; L(di/dt)�t�B���^ */
	VUSHORT	MREG_IDFH;				/* 0x70C2 ; */
	VUSHORT	MREG_IQFL;				/* 0x70C4 ; */
	VUSHORT	MREG_IQFH;				/* 0x70C6 ; */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG100;			/* 0x70C8 ; Work  Register 100 */
	VUSHORT	MREG_WREG101;			/* 0x70CA ; Work  Register 101 */
	VUSHORT	MREG_WREG102;			/* 0x70CC ; Work  Register 102 */
	VUSHORT	MREG_WREG103;			/* 0x70CE ; Work  Register 103 */
	VUSHORT	MREG_WREG104;			/* 0x70D0 ; Work  Register 104 */
	VUSHORT	MREG_KF31;				/* 0x70D2 ; */
	VUSHORT	MREG_KF32;				/* 0x70D4 ; */
	VUSHORT	MREG_KF33;				/* 0x70D6 ; */
	VUSHORT	MREG_KF34;				/* 0x70D8 ; */
	VUSHORT	MREG_WREG109;			/* 0x70DA ; Work  Register 109	�� �g���ĂȂ� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy3[6];			/* 0x70DC - 0x70E6 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VMAX2;				/* 0x70E8 ; �d���w�߃x�N�g���ő�l(8192*1.27) */
	VUSHORT	MREG_V12;				/* 0x70EA ; �d���w�߃x�N�g��(��(VD^2+VQ^2)) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy4[10];		/* 0x70EC - 0x70FE */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_FLG;				/* 0x7100 ; �ϕ��O�a�׸� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy5;			/* 0x7102 ; */
/*--------------------------------------------------------------------------------------------------*/
/*		for INT_HOST																				*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_HTMP0;				/* 0x7104 ; HOST Interrupt Temp0(130) */
	VUSHORT	MREG_HTMP1;				/* 0x7106 ; HOST Interrupt Temp1(131) */
	VUSHORT	MREG_HTMP2;				/* 0x7108 ; HOST Interrupt Temp2(132) */
	VUSHORT	MREG_HTMP3;				/* 0x710A ; HOST Interrupt Temp3(133) */
	VUSHORT	MREG_HTMP4;				/* 0x710C ; HOST Interrupt Temp4(134) */
	VUSHORT	MREG_HTMP5;				/* 0x710E ; HOST Interrupt Temp5(135) */
	VUSHORT	MREG_HTMP6;				/* 0x7110 ; HOST Interrupt Temp6(136) */
	VUSHORT	MREG_HTMP7;				/* 0x7112 ; HOST Interrupt Temp7(137) */
	VUSHORT	MREG_HTMP8;				/* 0x7114 ; HOST Interrupt Temp8(138) */
	VUSHORT	MREG_HTMP9;				/* 0x7116 ; HOST Interrupt Temp9(139) */
/*--------------------------------------------------------------------------------------------------*/
/*		for ROUND																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG140;			/* 0x7118 ; Work  Register 140 */
	VUSHORT	MREG_WREG141;			/* 0x711A ; Work  Register 141 */
	VUSHORT	MREG_WREG142;			/* 0x711C ; Work  Register 142 */
	VUSHORT	MREG_WREG143;			/* 0x711E ; Work  Register 143 */
	VUSHORT	MREG_WREG144;			/* 0x7120 ; Work  Register 144 */
	VUSHORT	MREG_WREG145;			/* 0x7122 ; Work  Register 145 */
	VUSHORT	MREG_WREG146;			/* 0x7124 ; Work  Register 146 */
	VUSHORT	MREG_WREG147;			/* 0x7126 ; Work  Register 147 */
	VUSHORT	MREG_WREG148;			/* 0x7128 ; Work  Register 148 */
	VUSHORT	MREG_WREG149;			/* 0x712A ; Work  Register 149 */
/*--------------------------------------------------------------------------------------------------*/
/*		for INT_AD																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_TMP0;				/* 0x712C ; */
	VUSHORT	MREG_TMP1;				/* 0x712E ; */
	VUSHORT	MREG_TMP2;				/* 0x7130 ; */
	VUSHORT	MREG_TMP3;				/* 0x7132 ; */
	VUSHORT	MREG_TMP4;				/* 0x7134 ; */
	VUSHORT	MREG_TMP5;				/* 0x7136 ; */
	VUSHORT	MREG_TMP6;				/* 0x7138 ; */
	VUSHORT	MREG_TMP7;				/* 0x713A ; */
	VUSHORT	MREG_TMP8;				/* 0x713C ; */
	VUSHORT	MREG_TMP9;				/* 0x713E ; */
/*--------------------------------------------------------------------------------------------------*/
/*		CPUk����̎󂯎�背�W�X�^																	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IUOFIN;			/* 0x7140 ; A/D transfer offset for iu */
	VUSHORT	MREG_IVOFIN;			/* 0x7142 ; A/D transfer offset for iv */
	VUSHORT	MREG_KCUIN;				/* 0x7144 ; A/D transfer gain for iu */
	VUSHORT	MREG_KCVIN;				/* 0x7146 ; A/D transfer gain for iv */
	VUSHORT	MREG_IDIN;				/* 0x7148 ; Id reference */
	VUSHORT	MREG_IQIN;				/* 0x714A ; Iq reference */
	VUSHORT	MREG_PHAIN;				/* 0x714C ; PHA input */
	VUSHORT	MREG_VELIN;				/* 0x714E ; VEL input */
/*--------------------------------------------------------------------------------------------------*/
/*		static variable																				*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KC;				/* 0x7150 ; Current conversion Gain */
	VUSHORT	MREG_LD;				/* 0x7152 ; d axis Inductance */
	VUSHORT	MREG_LQ;				/* 0x7154 ; q axis Inductance */
	VUSHORT	MREG_MAG;				/* 0x7156 ; Magnetic flux (Phi) */
	VUSHORT	MREG_KDP;				/* 0x7158 ; d axis propotion gain (PI control) */
	VUSHORT	MREG_KQP;				/* 0x715A ; q axis propotion gain (PI control) */
	VUSHORT	MREG_KDI;				/* 0x715C ; d axis integral time (gain) */
	VUSHORT	MREG_KQI;				/* 0x715E ; q axis integral time (gain) */
	VUSHORT	MREG_VDLIM;				/* 0x7160 ; d axis integral limit */
	VUSHORT	MREG_VQLIM;				/* 0x7162 ; q axis integral limit */
	VUSHORT	MREG_KVVIN;				/* 0x7164 ; Voltage Compasation gain */
	VUSHORT	MREG_ODLVL;				/* 0x7166 ; On delay change level(97.5.26 mo) */
	VUSHORT	MREG_TFIL;				/* 0x7168 ; Filter time constant */
	VUSHORT	MREG_VMAX;				/* 0x716A ; voltage limit data (Vmax^2) */
	VUSHORT	MREG_ONDV;				/* 0x716C ; On-delay compensation */
	VUSHORT	MREG_CSW;				/* 0x716E ; Control Flag */
	VUSHORT	MREG_CRFQ;				/* 0x7170 ; carrier freq reference(change while BB) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy6[3];			/* 0x7172 - 0x7176 */
/*--------------------------------------------------------------------------------------------------*/
/*		voltage compensation																		*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VDRIN;				/* 0x7178 ; vdref input */
	VUSHORT	MREG_VQRIN;				/* 0x717A ; vqref input */
/*--------------------------------------------------------------------------------------------------*/
/*		notch1																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KF1;				/* 0x717C */
	VUSHORT	MREG_KF2;				/* 0x717E */
	VUSHORT	MREG_KF3;				/* 0x7180 */
	VUSHORT	MREG_KF4;				/* 0x7182 */
/*--------------------------------------------------------------------------------------------------*/
/*		notch2																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KF21;				/* 0x7184 */
	VUSHORT	MREG_KF22;				/* 0x7186 */
	VUSHORT	MREG_KF23;				/* 0x7188 */
	VUSHORT	MREG_KF24;				/* 0x718A */
/*--------------------------------------------------------------------------------------------------*/
/*		LPF																							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_TLPF;				/* 0x718C ; LPF1 gain */
	VUSHORT	MREG_TLPFC;				/* 0x718E ; LPF2 gain */
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_TLMTPIN;			/* 0x7190 ; +tlim */
	VUSHORT	MREG_TLMTMIN;			/* 0x7192 ; -tlim */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_R1;				/* 0x7194 ; Moter resistance(r1/I_BASE*V_BASE) */
	VUSHORT	MREG_KONDV;				/* 0x7196 ; Dead-time comp. gain */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy7[2];			/* 0x7198 - 0x719A */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_L_DIDT;			/* 0x719C ; */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_FCCRST;			/* 0x719E ; FCCST Reset(for TEST) */


/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
// �p�����[�^�p
	VUSHORT	MREG_TSPL;				/* 0x71A0 ; Ts/L ( �I�u�U�[�o�p�p�����[�^ ) */
	VUSHORT	MREG_GOBS;				/* 0x71A2 ; g ( �I�u�U�[�o�̋� ) */
	VUSHORT	MREG_RLTS;				/* 0x71A4 ; 1-R�Ts/L ( �I�u�U�[�o�p�p�����[�^ ) */
	VUSHORT	MREG_FILOBS;			/* 0x71A6 ; �t�B���^�Q�C�� */
// �v�Z�p
	VUSHORT	MREG_LFILQL;			/* 0x71A8 ; q�����[�p�X�t�B���^�ϐ� ( Low byte ) */
	VUSHORT	MREG_LFILQH;			/* 0x71AA ; q�����[�p�X�t�B���^�ϐ� ( High byte ) */
	VUSHORT	MREG_HFILQL;			/* 0x71AC ; q���n�C�p�X�t�B���^�ϐ� ( Low byte ) */
	VUSHORT	MREG_HFILQH;			/* 0x71AE ; q���n�C�p�X�t�B���^�ϐ� ( High byte ) */
	VUSHORT	MREG_IQOBS;				/* 0x71B0 ; q���I�u�U�[�o�o�� */
	VUSHORT	MREG_IQOBSF;			/* 0x71B2 ; q���U������ */
	VUSHORT	MREG_LFILDL;			/* 0x71B4 ; d�����[�p�X�t�B���^�ϐ� ( Low byte ) */
	VUSHORT	MREG_LFILDH;			/* 0x71B6 ; d�����[�p�X�t�B���^�ϐ� ( High byte ) */
	VUSHORT	MREG_HFILDL;			/* 0x71B8 ; d���n�C�p�X�t�B���^�ϐ� ( Low byte ) */
	VUSHORT	MREG_HFILDH;			/* 0x71BA ; d���n�C�p�X�t�B���^�ϐ� ( High byte ) */
	VUSHORT	MREG_IDOBS;				/* 0x71BC ; d���I�u�U�[�o�o�� */
	VUSHORT	MREG_IDOBSF;			/* 0x71BE ; d���U������ */
	VUSHORT	MREG_DMPG;				/* 0x71C0 ; �d���I�u�U�[�o�_���s���O�Q�C�� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy8[31];		/* 0x71C2 - 0x71FE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Phase Interpolate			<V112>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_PHAIP;				/* 0x7200 ; �ʑ���ԗ� */
	VUSHORT	MREG_PHAIPFLG;			/* 0x7202 ; �ʑ���ԃt���O(0�F���Ȃ��A1�F����) */
	VUSHORT	MREG_PHAIPIN;			/* 0x7204 ; �ʑ���ԗ�(CPU��ASIC�󂯓n���p) */
	VUSHORT	MREG_PHAIPFLGIN;		/* 0x7206 ; �ʑ���ԃt���O(CPU��ASIC�󂯓n���p) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy9[12];		/* 0x7208 - 0x721E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Encoder IF																		<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_AMPTYPE;			/* 0x7220 ; Rotary Type or Linear Type */
	VUSHORT	MREG_Dummy10;			/* 0x7222 ; Dummy */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_RCVPOSX0;			/* 0x7224 ; Motor Encoder Position(��M�ʒu) Low */
	VUSHORT	MREG_RCVPOSX0H;			/* 0x7226 ; Motor Encoder Position(��M�ʒu) High */
	VUSHORT	MREG_RCVPOSX1;			/* 0x7228 ; Motor Encoder Position(��M�ʒu) Low */
	VUSHORT	MREG_RCVPOSX1H;			/* 0x722A ; Motor Encoder Position(��M�ʒu) High */
	VUSHORT	MREG_RCVPOSX2;			/* 0x722C ; Motor Encoder Position(��M�ʒu) Low */
	VUSHORT	MREG_RCVPOSX2H;			/* 0x722E ; Motor Encoder Position(��M�ʒu) High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_MPOSSFTR;			/* 0x7230 ; Shift Data for Motor Position Cal. */
	VUSHORT	MREG_MPOSSFTX;			/* 0x7232 ; Shift Data for Motor Position Cal. */
	VUSHORT	MREG_MPOSSIGN;			/* 0x7234 ; Sign  Data for Motor Position Cal. */
	VUSHORT	MREG_DIVOUTSFT;			/* 0x7236 ; �����o�̓V�t�g�� */
	VUSHORT	MREG_ACCERRLVL;			/* 0x7238 ; Motor Encoder Acc. Error Check Low */
	VUSHORT	MREG_ACCERRLVLH;		/* 0x723A ; Motor Encoder Acc. Error Check High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_DIVPLS;			/* 0x723C ; �����o�̓p���X Low */
	VUSHORT	MREG_DIVPLSH;			/* 0x723E ; �����o�̓p���X High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_DIVOUTGAIN;		/* 0x7240 ; �����o�̓Q�C��(���j�A) Low */
	VUSHORT	MREG_DIVOUTGAINH;		/* 0x7242 ; �����o�̓Q�C��(���j�A) High */
	VUSHORT	MREG_DIVPOS;			/* 0x7244 ; �����o�̓p���X(���j�A) Low */
	VUSHORT	MREG_DIVPOSH;			/* 0x7246 ; �����o�̓p���X(���j�A) High */
	VUSHORT	MREG_DIVREM;			/* 0x7248 ; �����o�̓p���X�]��(���j�A) Low */
	VUSHORT	MREG_DIVREMH;			/* 0x724A ; �����o�̓p���X�]��(���j�A) High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_SPGFAIL;			/* 0x724C ; S-PG��M���s���e�� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_BITIPRM;			/* 0x724E ; Bit Initial Parameter */
	VUSHORT	MREG_BITDAT;			/* 0x7250 ; Bit Data */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_INCPLSREQ;			/* 0x7252 ; PAO IncrePulse Output Request */
	VUSHORT	MREG_PAOSEQCMD;			/* 0x7254 ; PAO Serial Output Sequence */
	VUSHORT	MREG_POSETCMD;			/* 0x7256 ; �p���X�o�͉�H�������v�� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy11[4];		/* 0x7258 - 0x725E */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊֘A�FHostCPU --> Asic ������]���p									<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_INCPLSREQIN;		/* 0x7260 ; PAO IncrePulse Output Request */
	VUSHORT	MREG_PAOSEQCMDIN;		/* 0x7262 ; PAO Serial Output Sequence */
	VUSHORT	MREG_POSETCMDIN;		/* 0x7264 ; �p���X�o�͉�H�������v�� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_DIVSETIN;			/* 0x7266 ; �����@�\�ݒ���� */
	VUSHORT	MREG_POSET1IN;			/* 0x7268 ; �p���X�ϊ����_�␳1 */
	VUSHORT	MREG_POSET2IN;			/* 0x726A ; �p���X�ϊ����_�␳2 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_INCPLSIN;			/* 0x726C ; �����C���N���p���X Low */
	VUSHORT	MREG_INCPLSINH;			/* 0x726E ; �����C���N���p���X High */
	VUSHORT	MREG_INCREMIN;			/* 0x7270 ; �����C���N���p���X�]�� Low */
	VUSHORT	MREG_INCREMINH;			/* 0x7272 ; �����C���N���p���X�]�� High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_ACCCNTCLRIN; 		/* 0x7274 ; �����x�`�F�b�N�J�n�J�E���g�N���A�v�� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy12[197];		/* 0x7276 - 0x73FE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for axis q monitor				<V224>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQDISTIN;			/* 0x7400 ; q���O���g���N���� */
	VUSHORT	MREG_IQDIST;			/* 0x7402 ; q���O���g���N */
	VUSHORT	MREG_IQMONFIL;			/* 0x7404 ; q���d���w�߃��j�^(�t�B���^��) */
	VUSHORT	MREG_IQOFREF;			/* 0x7406 ; q���d���w��(�O���g���N���Z��) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy13[76];		/* 0x7408 - 0x749E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO1L;				/* 0x74A0 ; filter1 output(Low) */
	VUSHORT	MREG_IQO1H;				/* 0x74A2 ; filter1 output(High) */
	VUSHORT	MREG_IQO1PL;			/* 0x74A4 ; filter1 output 1delay(Low) */
	VUSHORT	MREG_IQO1PH;			/* 0x74A6 ; filter1 output 1delay(High) */
	VUSHORT	MREG_IQO1PPL;			/* 0x74A8 ; filter1 output 2delay(Low) */
	VUSHORT	MREG_IQO1PPH;			/* 0x74AA ; filter1 output 2delay(High) */
	VUSHORT	MREG_IQIPL;				/* 0x74AC ; filter1 input 1delay(Low) */
	VUSHORT	MREG_IQIPH;				/* 0x74AE ; filter1 input 1delay(High) */
	VUSHORT	MREG_IQIPPL;			/* 0x74B0 ; filter1 input 2delay(Low) */
	VUSHORT	MREG_IQIPPH;			/* 0x74B2 ; filter1 input 2delay(High) */
	VUSHORT	MREG_IQO1S;				/* 0x74B4 ; filter1 output(short type) */
	VUSHORT	MREG_IQO1SH;			/* 0x74B6 ; filter1 output(short type) */
	VUSHORT	MREG_IQO1BUFL;			/* 0x74B8 ; filter1 output buffer(Low) */
	VUSHORT	MREG_IQO1BUFH;			/* 0x74BA ; filter1 output buffer(Low) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy14[2];		/* 0x74BC - 0x74BE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO2L;				/* 0x74C0 ; filter2 output(Low) */
	VUSHORT	MREG_IQO2H;				/* 0x74C2 ; filter2 output(High) */
	VUSHORT	MREG_IQO2PL;			/* 0x74C4 ; filter2 output 1delay(Low) */
	VUSHORT	MREG_IQO2PH;			/* 0x74C6 ; filter2 output 1delay(High) */
	VUSHORT	MREG_IQO2PPL;			/* 0x74C8 ; filter2 output 2delay(Low) */
	VUSHORT	MREG_IQO2PPH;			/* 0x74CA ; filter2 output 2delay(High) */
	VUSHORT	MREG_IQI2PL;			/* 0x74CC ; filter2 input 1delay(Low) */
	VUSHORT	MREG_IQI2PH;			/* 0x74CE ; filter2 input 1delay(High) */
	VUSHORT	MREG_IQI2PPL;			/* 0x74D0 ; filter2 input 2delay(Low) */
	VUSHORT	MREG_IQI2PPH;			/* 0x74D2 ; filter2 input 2delay(High) */
	VUSHORT	MREG_IQO2S;				/* 0x74D4 ; filter2 output(short type) */
	VUSHORT	MREG_IQO2SH;			/* 0x74D6 ; filter2 output(short type) */
	VUSHORT	MREG_IQO2BUFL;			/* 0x74D8 ; filter2 output buffer(Low) */
	VUSHORT	MREG_IQO2BUFH;			/* 0x74DA ; filter2 output buffer(Low) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy15[2];		/* 0x74DC - 0x74DE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter3 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO3L;				/* 0x74E0 ; filter3 output(Low) */
	VUSHORT	MREG_IQO3H;				/* 0x74E2 ; filter3 output(High) */
	VUSHORT	MREG_IQO3PL;			/* 0x74E4 ; filter3 output 1delay(Low) */
	VUSHORT	MREG_IQO3PH;			/* 0x74E6 ; filter3 output 1delay(High) */
	VUSHORT	MREG_IQO3PPL;			/* 0x74E8 ; filter3 output 2delay(Low) */
	VUSHORT	MREG_IQO3PPH;			/* 0x74EA ; filter3 output 2delay(High) */
	VUSHORT	MREG_IQI3PL;			/* 0x74EC ; filter3 input 1delay(Low) */
	VUSHORT	MREG_IQI3PH;			/* 0x74EE ; filter3 input 1delay(High) */
	VUSHORT	MREG_IQI3PPL;			/* 0x74F0 ; filter3 input 2delay(Low) */
	VUSHORT	MREG_IQI3PPH;			/* 0x74F2 ; filter3 input 2delay(High) */
	VUSHORT	MREG_IQO3S;				/* 0x74F4 ; filter3 output(short type) */
	VUSHORT	MREG_IQO3SH;			/* 0x74F6 ; filter3 output(short type) */
	VUSHORT	MREG_IQO3BUFL;			/* 0x74F8 ; filter3 output buffer(Low) */
	VUSHORT	MREG_IQO3BUFH;			/* 0x74FA ; filter3 output buffer(Low) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy16[130];		/* 0x74FC - 0x75FE */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Weaken Field		<V214>																*/
/*--------------------------------------------------------------------------------------------------*/
//----- CPU����n�����ϐ� -----
	VUSHORT	MREG_WFKPVLIN;			/* 0x7600 ; �d��FB���Q�C��(����16bit) */
	VUSHORT	MREG_WFKPVHIN;			/* 0x7602 ; �d��FB���Q�C��(���16bit) */
	VUSHORT	MREG_WFKIVLIN;			/* 0x7604 ; �d��FB�ϕ��Q�C��(����16bit) */
	VUSHORT	MREG_WFKIVHIN;			/* 0x7606 ; �d��FB�ϕ��Q�C��(���16bit) */
	VUSHORT	MREG_WFV1MAXIN;			/* 0x7608 ; �d���w�ߐ������x�� */
	VUSHORT	MREG_WFIDREFLIMIN;		/* 0x760A ; d���d���w�߃��~�b�g */

//----- CPU���珉�������ɓn�����ϐ� -----
	VUSHORT	MREG_WFINTEGLIM;		/* 0x760C 	; �d��FB�ϕ����~�b�g */
	VUSHORT	MREG_Dummy17;			/* 0x760E 	; Dummy */

//----- �}�C�N���ň����ϐ� -----
	VUSHORT	MREG_WFKPVL;			/* 0x7610 ; �d��FB���Q�C��(����16bit) */
	VUSHORT	MREG_WFKPVH;			/* 0x7612 ; �d��FB���Q�C��(���16bit) */
	VUSHORT	MREG_WFKIVL;			/* 0x7614 ; �d��FB�ϕ��Q�C��(����16bit) */
	VUSHORT	MREG_WFKIVH;			/* 0x7616 ; �d��FB�ϕ��Q�C��(���16bit) */
	VUSHORT	MREG_WFV1MAX;			/* 0x7618 ; �d���w�ߐ������x�� */
	VUSHORT	MREG_WFIDREFLIM;		/* 0x761A ; d���d���w�߃��~�b�g */
	VUSHORT	MREG_WFVQMAX;			/* 0x761C ; q���d���w�߃��~�b�g */
	VUSHORT	MREG_Dummy18;			/* 0x761E ; Dummy */
	VUSHORT	MREG_WFINTEGL;			/* 0x7620 ; �d��FB�ϕ��l */
	VUSHORT	MREG_WFINTEGH;			/* 0x7622 ; �d��FB�ϕ��l */
	VUSHORT	MREG_WFVDREF;			/* 0x7624 ; d���d���w�� */
	VUSHORT	MREG_WFVQREF;			/* 0x7626 ; q���d���w�� */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy19[236];		/* 0x7628 - 0x77FE */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		�}�C�N�����Z���\�t�g�o�[�W������� : �}�C�N�����Z�����z�X�gCPU�]���p				<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_MSWVERSION;		/* 0x7800 ; �\�t�g�o�[�W������� */
	VUSHORT	MREG_TSTVERSION;		/* 0x7802 ; �e�X�g�o�[�W������� */
	VUSHORT	MREG_YSPVERSION;		/* 0x7804 ; �x�d�l�o�[�W������� */

}ASIC_U_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		�r�b�g�p�����[�^ ( ���������Z�b�g CPU��ASIC )												*/
/****************************************************************************************************/
typedef struct MICRO_BITIPRM
{
	USHORT	data;
} MICRO_BITIPRM;
#define		BITIPRM_SGM3ENC			BIT0	/* bit0  : Sigma-3 Encoder								*/
#define		BITIPRM_IPSPDCONTROL	BIT1	/* bit1  : IP Speed Control								*/
#define		BITIPRM_SPDICTRLKEEP	BIT2	/* bit2  : Speed Integral Control Keep					*/
#define		BITIPRM_DIVOUT			BIT3	/* bit3  : ���[�^�G���R�[�_�����o�͑I��					*/
#define		BITIPRM_RVSDIR			BIT4	/* bit4  : �t��]���[�h									*/
#define		BITIPRM_SERIALCONV		BIT5	/* bit5  : Use Serial Convert Unit						*/
#define		BITIPRM_POLESENSOR		BIT6	/* bit6  : Select Pole Sensor(1:�|�[���Z���T����)		*/
#define		BITIPRM_PHSUWV			BIT7	/* bit7  : �p���[�o�͑����t�v�u(1:UWV����)				*/
#define		BITIPRM_EHSPDOBSON		BIT8	/* bit8  : �ʑ��⏞���x�I�u�U�[�o�I��(1:�L��)			*/
#define		BITIPRM_TUNELESSON		BIT9	/* bit9  : �������X�I��(1:�L��)							*/
#define		BITIPRM_TLSPDPHACOMP	BIT10	/* bit10 : �������X���x���䎞�ʑ��⏞(1:�L��)			*/
#define		BITIPRM_FORCETLINVLD	BIT11	/* bit11 : �������X���������v��(1:��������)				*/
#define		BITIPRM_TLSPDDETCHNG	BIT12	/* bit12 : �������X���x���o�؂�ւ��ݒ�(1:�؂�ւ�����)	*/
#define		BITIPRM_LPASSFIL3ON		BIT13	/* bit13 : �g���N�⏞��LPF�v�� (1:�g���N�⏞��LPF����)	*/
#define		BITIPRM_MOTWEAKENFIELD	BIT14	/* bit14 : ���[�^��ߊE���I��							*/
#define		BITIPRM_CARRERCHNG		BIT15	/* bit15 : ��~���L�����A�؂�ւ��I��(1:�L��)			*/



/****************************************************************************************************/
/*		�r�b�g�f�[�^ ( ���������Z�b�g CPU��ASIC�A�X�L�������� ASIC��CPU )							*/
/****************************************************************************************************/
typedef struct MICRO_BITDAT
{
	USHORT	data;
} MICRO_BITDAT;
#define		BITDAT_SERXERR			BIT0	/* bit0  : Senc Receive Error							*/
#define		BITDAT_CSET				BIT1	/* bit1  : Cphase Set									*/
#define		BITDAT_CPASS			BIT2	/* bit2  : Cphase Pass									*/
#define		BITDAT_ORGOK			BIT3	/* bit3  : Motor Origin OK								*/
#define		BITDAT_DIVCPASS			BIT4	/* bit4  : DivCphase Pass								*/
#define		BITDAT_CPOSLATCH		BIT5	/* bit5  : Cphase Position Latch Signal					*/
#define		BITDAT_ACCCHKENABLE		BIT6	/* bit6  : Encoder Acc. Check Enable Flag				*/
#define		BITDAT_PCONACTFLG		BIT7	/* bit7  : P�����ԃt���O								*/
#define		BITDAT_SVONSPDLMT		BIT8	/* bit8  : �T�[�{ON�����x������Flag						*/
#define		BITDAT_SPDCLAMP			BIT9	/* bit9  : �g���N���䎞���x������Flag					*/
#define		BITDAT_ENCDATANG		BIT10	/* bit10 : �G���R�[�_�f�[�^����Flag						*/
#define		BITDAT_AC90ALM			BIT11	/* bit11 : A.C90(�G���R�[�_�ʐM�ُ�)					*/
#define		BITDAT_AC91ALM			BIT12	/* bit12 : A.C91(�G���R�[�_�ʐM�ʒu�f�[�^�����x�ُ�)	*/
#define		BITDAT_AC20ALM			BIT13	/* bit13 : A.C20(�ʑ��댟�o)							*/
#define		BITDAT_AC21ALM			BIT14	/* bit14 : A.C21(�|�[���Z���T�ُ�)						*/
#define		BITDAT_ABF3ALM			BIT15	/* bit15 : A.BF3(ScanA�����ُ�)							*/



/****************************************************************************************************/
/*		�r�b�g�f�[�^1 ( ASIC��p )																	*/
/****************************************************************************************************/
typedef struct MICRO_BITDAT1
{
	USHORT	data;
} MICRO_BITDAT1;
#define		BITDAT1_HENKATSPDCMP	BIT0	/* bit0  : �ω��ʒu�^�C�}�ɂ�鑬�x�␳�t���O			*/
#define		BITDAT1_SPDMAFILFLG		BIT1	/* bit1  : ���[�^���x�̈ړ����Ϗ����p�t���O				*/
#define		BITDAT1_MODFLG			BIT2	/* bit2  : �x�[�X�^�C�}���x���Z���s�t���O				*/
#define		BITDAT1_LASTMODFLG		BIT3	/* bit3  : �O��x�[�X�^�C�}���x���Z���s�t���O			*/
#define		BITDAT1_LASTSERXERR		BIT4	/* bit4  : Last Senc Receive Error						*/
#define		BITDAT1_PERCLRSTS		BIT5	/* bit5  : Perr Clear Status							*/
//									BIT6	/* bit6  : 												*/
//									BIT7	/* bit7  : 												*/
//									BIT8	/* bit8  : 												*/
//									BIT9	/* bit9  : 												*/
//									BIT10	/* bit10 : 												*/
//									BIT11	/* bit11 : 												*/
//									BIT12	/* bit12 : 												*/
//									BIT13	/* bit13 : 												*/
//									BIT14	/* bit14 : 												*/
//									BIT15	/* bit15 : 												*/



/****************************************************************************************************/
/*		�r�b�g�R�}���h�M�� ( �X�L�������� CPU��ASIC )												*/
/****************************************************************************************************/
typedef struct MICRO_BITCMD
{
	USHORT	data;
} MICRO_BITCMD;
#define		BITCMD_PCON				BIT0	/* bit0  : Pcon											*/
#define		BITCMD_MODESWON			BIT1	/* bit1  : Mode Switch On								*/
#define		BITCMD_PERCLRCMD		BIT2	/* bit2  : Perr Clear Command							*/
#define		BITCMD_PICVCLRCMD		BIT3	/* bit3  : Position Integral Control Data Clear Command	*/
#define		BITCMD_SCANAPASSREQ		BIT4	/* bit4  : Pass through ScanA process Command			*/
#define		BITCMD_TBLDRVON			BIT5	/* bit5  : Table Drive Request  (1:�e�[�u���^�])		*/
#define		BITCMD_CLATCHCLR		BIT6	/* bit6  : Cphase Latch Clear Request  (1:C�����b�`��ԃN���A)	*/
#define		BITCMD_TUNELESSREQ		BIT7	/* bit7  : Tuning Less Control Request  (1:�������X�L��)	*/
#define		BITCMD_DOBSON			BIT8	/* bit8  : ���C�⏞�I��  (1:���C�⏞�L��)				*/
#define		BITCMD_AUTOGAINCHNG		BIT9	/* bit9  : �����Q�C���؂�ւ��I��  (1:�����Q�C���؂�ւ�)	*/
#define		BITCMD_GAINCHNGON		BIT10	/* bit10 : �Q�C���؂�ւ��v��  (1:���Q�C���L��)		*/
#define		BITCMD_BBSVCREFCLR		BIT11	/* bit11 : BBSvcRefClr Request Flag  (1:�w�߃N���A)		*/
#define		BITCMD_TRQFILINVLD		BIT12	/* bit12 : �g���N�w�߃t�B���^�����I��  (1:�t�B���^�Ȃ�)	*/
#define		BITCMD_LPFINTEGINIT		BIT13	/* bit13 : �ꎟLPF�ϕ��������v��  (1:�ϕ�����������)	*/
#define		BITCMD_LPASSFIL2ON		BIT14	/* bit14 : ��LPF�v��  (1:��LPF����)					*/
//									BIT15	/* bit15 : 												*/



/****************************************************************************************************/
/*		��������w��r�b�g��` (MREG_CSW)															*/
/****************************************************************************************************/
typedef struct MICRO_CSW
{
	USHORT	data;
} MICRO_CSW;
#define		CSW_ICLR				BIT0	/* bit0  : �ϕ����~�b�g�������F�z�[���h(0)�^�N���A(1)	*/
#define		CSW_ISEL				BIT1	/* bit1  : Ld/Lq�⏞�F�d��FB(0)�^�d���w��(1)			*/
//									BIT2	/* bit2  : --Reserve--									*/
#define		CSW_OBSSEL				BIT3	/* bit3  : Current Observer Select bit					*/
#define		CSW_F1DSABL				BIT4	/* bit4  : Notch filter1 Disable						*/
#define		CSW_F2DSABL				BIT5	/* bit5  : Notch filter2 Disable						*/
#define		CSW_LPFDSABL			BIT6	/* bit6  : Low Pass filter1 Disable						*/
#define		CSW_LPFCDSABL			BIT7	/* bit7  : Low Pass filter2 Disable						*/
#define		CSW_OVMSEL1				BIT8	/* bit8  : Over modulation select(bit 8,9)				*/
#define		CSW_OVMSEL2				BIT9	/* bit9  : if bit8,9 = 10 or 11 then OVMOD2  if bit8,9 = 01 then OVMOD1	*/
#define		CSW_OVMMOD				BIT10	/* bit10 : modulation modification enable				*/
#define		CSW_V_FB				BIT11	/* bit11 : �d��FB������ߊE���I��						*/
#define		CSW_DIDTSEL				BIT12	/* bit12 : L di/dt cmp select(1:di/dt,0:def)			*/
#define		CSW_V_FB2				BIT13	/* bit13 : �d��FB������ߊE���I��2						*/
#define		CSW_INT_ST				BIT14	/* bit14 : �O�a���ϕ���~								*/
#define		CSW_F3DSABL				BIT15	/* bit15 : Notch filter3 Disable						*/



/****************************************************************************************************/
/*		�ʃv���O���������`																		*/
/****************************************************************************************************/
/*		Bit-0 : �ʃv���O�������s(1)�^��~(0)														*/
/*		Bit-3 : ���W�X�^���b�N(�d�����o�����p)														*/
/*		Bit-4 : �x�[�X�u���b�N(�T�[�{�n�e�e)														*/
/****************************************************************************************************/
#define	MPCMD_PRGSTOP		0x0000			/* �ʃv���O�������s��~									*/
#define	MPCMD_PRGSTART		0x0011			/* �ʃv���O�������s�J�n									*/
#define	MPCMD_SVON			0x0001			/* �ʃv���O�����T�[�{ON									*/
#define	MPCMD_SVOFF			0x0011			/* �ʃv���O�����T�[�{OFF								*/



/****************************************************************************************************/
/*		����w�߃R�}���h��`  MREG_HSUR																*/
/****************************************************************************************************/
/*		MPCMD_SYNC1      Bit-0 : �����t���O1(�ʏ�̍X�V�p)											*/
/*		MPCMD_SYNC2      Bit-1 : �����t���O2(�ʑ��̂ݍX�V�p)										*/
/*		MPCMD_RENEWAL    Bit-2 : �ʃv���O�����w�ߍX�V												*/
/*		MPCMD_CURADSYNC  Bit-3 : �d�����oAD���荞�ݓ���												*/
/****************************************************************************************************/
#define	MPCMD_SYNC1			0x0001			/* 														*/
#define	MPCMD_SYNC2			0x0002			/* �[������Ȃ�											*/
#define	MPCMD_RENEWAL		0x0004			/* �[������~���[�h										*/
#define	MPCMD_CURADSYNC		0x0008			/* �[���N�����v���[�h									*/



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-076B/077B ���W�X�^�A�N�Z�X�p�\���̒�`												*/
/*																									*/
/****************************************************************************************************/
typedef struct  {
	ASIC_HW_IF_STR		*AsicHwReg;			/* ASIC H/W���W�X�^										*/
	ASIC_U_IF_STR		*AsicMicroReg;		/* ASIC Micro���W�X�^									*/
	ASIC_HW_IF_STR		*AsicComHwReg;		/* ASIC H/W���W�X�^(�S�������A�N�Z�X)					*/
	ASIC_U_IF_STR		*AsicComMicroReg;	/* ASIC Micro���W�X�^(�S�������A�N�Z�X)					*/
	MICRO_BITIPRM		MicroBitIprm;		/* MicroBitIprm���W�X�^									*/
	MICRO_BITDAT		MicroBitDat;		/* MicroBitDat���W�X�^									*/
	MICRO_BITCMD		MicroBitCmd;		/* MicroBitCmd���W�X�^									*/
	MICRO_CSW			MicroCsw;			/* MicroCsw���W�X�^										*/
	BOOL				MicroRunningSts;	/* Micro�X�e�[�^�X�m�F�p								*/
	MICRO_CYC_OUTPUTS	*uDMAOutBuffer;		/* Micro DMA Output buffer								*/
	MICRO_CYC_INPUTS	*uDMAInBuffer;		/* Micro DMA Input buffer								*/

#if defined(_VC_DEBUG_)
	USHORT				*pDummyMicroSram;	/* Dummy buffer for Micro Program download				*/
#endif /* _VC_DEBUG_ */

} ASICS;



/***************************************** end of file **********************************************/
#define _JL076REG_H // �ēǂݍ��ݖh�~�p
#endif
