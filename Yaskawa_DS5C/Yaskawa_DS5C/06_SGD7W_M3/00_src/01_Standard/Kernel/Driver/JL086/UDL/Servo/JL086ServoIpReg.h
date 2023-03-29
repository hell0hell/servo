/****************************************************************************************************/
/*																									*/
/*																									*/
/*		JL086ServoIpReg.h : JL-086 SERVOIP���W�X�^��`�w�b�_�t�@�C��								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. JL-086 SERVOIP���W�X�^��`																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2014.02.19 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef _JL086_SERVOIP_REG_H
#define _JL086_SERVOIP_REG_H



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-086 SERVOIP H/W���W�X�^�\���̒�`													*/
/*																									*/
/****************************************************************************************************/
typedef struct _ASIC_HW_IF_STR {
	VULONG	AREG_PWMU;				/* 0x0000 �A�i���O���j�^PWM�o��U								*/
	VULONG	AREG_PWMV;				/* 0x0004 �A�i���O���j�^PWM�o��V								*/
	VULONG	AREG_IOSTAT;			/* 0x0008 �ėp�����V���A���o�́����M�o�b�t�@�]��				*/
	VULONG	SPARE0;					/* 0x000C Reserve												*/
	VULONG	AREG_ITDO;				/* 0x0010 ���������p�^�C�}(RD)									*/
	VULONG	AREG_TIM1SET;			/* 0x0014 �^�C�}1�ݒ背�W�X�^									*/
	VULONG	AREG_UARTSTS;			/* 0x0018 UART�X�e�[�^�X										*/
	VULONG	AREG_UARTXD;			/* 0x001C �ėp�񓯊��|�[�g���M�f�[�^(WR)����M�f�[�^(RD)		*/
	VULONG	AREG_DIVCSEV;			/* 0x0020 �G���R�[�_�p���X�o�͕�����ݒ�						*/
	VULONG	SPARE1[4];				/* 0x0024-0x0030 Reserve										*/
	VULONG	AREG_INT1SET;			/* 0x0034 INT1�����ݐݒ�&�A�N�m���b�W							*/
	VULONG	AREG_PFNCSET;			/* 0x0038 �[�q�@�\�ݒ�											*/
	VULONG	SPARE2;					/* 0x003C Reserve												*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_FBCSET;			/* 0x0040 �������̓t�B���^�ݒ�									*/
	VULONG	AREG_POCSET;			/* 0x0044 �w�߃J�E���^�ݒ�										*/
	VULONG	AREG_DINCSET;			/* 0x0048 �����@�\�ݒ�											*/
	VULONG	AREG_LODSET;			/* 0x004C ���[�h�ݒ�											*/
	VULONG	AREG_T_FBLD;			/* 0x0050 FB�J�E���^�ݒ�										*/
	VULONG	AREG_T_CMDLD;			/* 0x0054 CMD�J�E���^�ݒ�										*/
	VULONG	AREG_ALMCLR;			/* 0x0058 �A���[�����b�`���Z�b�g								*/
	VULONG	AREG_ALMSET;			/* 0x005C �A���[���ݒ�											*/
	VULONG	AREG_IOSTATX;			/* 0x0060 �ėp����												*/
	VULONG	AREG_FLTSTAT;			/* 0x0064 �̏����												*/
	VULONG	SPARE3;					/* 0x0068 Reserve												*/
	VULONG	AREG_SETREG1;			/* 0x006C �����ݒ背�W�X�^										*/
	VULONG	AREG_SETREG2;			/* 0x0070 �@�\�I�����W�X�^										*/
	VULONG	AREG_CLKO;				/* 0x0074 �V���A���G���R�[�_IF�`�����x�ݒ背�W�X�^				*/
	VULONG	AREG_CSTP;				/* 0x0078 �@�\��~�I�����W�X�^("1"�Œ�~)						*/
	VULONG	AREG_SEPGSET00;			/* 0x007C �V���A���G���R�[�_IF�ݒ�FCH0�ݒ�0					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_SETREG3;			/* 0x0080 �񐶋@�\�ݒ背�W�X�^									*/
	VULONG	AREG_SEPGSET01;			/* 0x0084 �V���A���G���R�[�_IF�ݒ�FCH0�ݒ�1					*/
	VULONG	AREG_SEPGSET10;			/* 0x0088 �V���A���G���R�[�_IF�ݒ�FCH1�ݒ�0					*/
	VULONG	AREG_SEPGSET11;			/* 0x008C �V���A���G���R�[�_IF�ݒ�FCH1�ݒ�1					*/
	VULONG	SPARE4;					/* 0x0090 Reserve												*/
	VULONG	AREG_FCCSET;			/* 0x0094 �w�߃t�B���^�ݒ背�W�X�^								*/
	VULONG	AREG_T_FLC;				/* 0x0098 �t���N���[�Y�h�J�E���^�ݒ�							*/
	VULONG	AREG_ESYNC;				/* 0x009C �V���A���G���R�[�_�����J�E���^�O������				*/
	VULONG	AREG_SE0_TXD01;			/* 0x00A0 �V���A���G���R�[�_CH0���M�f�[�^0,1					*/
	VULONG	AREG_SE0_TXD23;			/* 0x00A4 �V���A���G���R�[�_CH0���M�f�[�^2,3					*/
	VULONG	AREG_SE0_TXD45;			/* 0x00A8 �V���A���G���R�[�_CH0���M�f�[�^4,5					*/
	VULONG	AREG_SE0_TXD67;			/* 0x00AC �V���A���G���R�[�_CH0���M�f�[�^6,7					*/
	VULONG	AREG_SE0_RXD01;			/* 0x00B0 �V���A���G���R�[�_CH0��M�f�[�^0,1					*/
	VULONG	AREG_SE0_RXD23;			/* 0x00B4 �V���A���G���R�[�_CH0��M�f�[�^2,3					*/
	VULONG	AREG_SE0_RXD45;			/* 0x00B8 �V���A���G���R�[�_CH0��M�f�[�^4,5					*/
	VULONG	AREG_SE0_RXD67;			/* 0x00BC �V���A���G���R�[�_CH0��M�f�[�^6,7					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	SPARE5[8];				/* 0x00C0-0x00DC Reserve										*/
	VULONG	AREG_GSSRD;				/* 0x00E0 �ėp�����V���A����M�f�[�^							*/
	VULONG	SPARE6;					/* 0x00E4 Reserve												*/
	VULONG	AREG_CMDCLML;			/* 0x00E8 �w�߃J�E���^CML���b�`�l								*/
	VULONG	AREG_SPRGCMD0;			/* 0x00EC SENC�FCH0�R�}���h/���X�|���X�o�b�t�@					*/
	VULONG	AREG_CDLTD;				/* 0x00F0 �V���A���G���R�[�_CH0�J�E���^���b�`�l					*/
	VULONG	AREG_CMDLDL;			/* 0x00F4 �w�߃J�E���^���[�h���b�`�l							*/
	VULONG	AREG_SPRGCMD1;			/* 0x00F8 SENC�FCH1�R�}���h/���X�|���X�o�b�t�@					*/
	VULONG	AREG_SEDLLD;			/* 0x00FC �V���A���G���R�[�_�x���������[�h�l					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	SPARE7[2];				/* 0x0100-0x0104 Reserve										*/
	VULONG	AREG_TSTEELWL;			/* 0x0108 EEPROM IF Low���[�h�l(�e�X�g�p)						*/
	VULONG	AREG_TSTDCCNT;			/* 0x010C ����DDA���ݒl											*/
	VULONG	AREG_TSTT1CNT;			/* 0x0110 �^�C�}T1�l(�e�X�g�p)									*/
	VULONG	SPARE8;					/* 0x0114 Reserve												*/
	VULONG	AREG_TSTPWMUCNT;		/* 0x0118 PWMU�J�E���^����8�r�b�g�l(�e�X�g�p)					*/
	VULONG	AREG_TSTPWMVCNT;		/* 0x011C PWMV�J�E���^����8�r�b�g�l(�e�X�g�p)					*/
	VULONG	AREG_SDMCONF1;			/* 0x0120 �f�V���[�V�����t�B���^�ݒ�							*/
	VULONG	AREG_SDMCONF2;			/* 0x0124 �I�[�o�����W���o�}�X�N�r�b�g�ݒ�A�����ݒ�			*/
	VULONG	AREG_PWMCCD;			/* 0x0128 PWM�L�����A�J�E���^���ݒl								*/
	VULONG	SPARE9;					/* 0x012C Reserve												*/
	VULONG	AREG_DYSETUP;			/* 0x0130 U��P���I���f�B���C���Ԑݒ�							*/
	VULONG	AREG_DYSETUN;			/* 0x0134 U��N���I���f�B���C���Ԑݒ�							*/
	VULONG	AREG_DYSETVP;			/* 0x0138 V��P���I���f�B���C���Ԑݒ�							*/
	VULONG	AREG_DYSETVN;			/* 0x013C V��N���I���f�B���C���Ԑݒ�							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_DYSETWP;			/* 0x0140 W��P���I���f�B���C���Ԑݒ�							*/
	VULONG	AREG_DYSETWN;			/* 0x0144 W��N���I���f�B���C���Ԑݒ�							*/
	VULONG	SPARE10[2];				/* 0x0148-0x014C Reserve										*/
	VULONG	AREG_ZDR;				/* 0x0150 �p���X�ϊ����_�␳�ݒ�								*/
	VULONG	AREG_ZENBR;				/* 0x0154 ���_�ʉ߃r�b�g�ʒu�ݒ�								*/
	VULONG	AREG_DIVHSR;			/* 0x0158 ������H�q�X�e���V�X�ݒ�								*/
	VULONG	SPARE11;				/* 0x015C Reserve												*/
	VULONG	AREG_PUMPON;			/* 0x0160 �`���[�W�|���v�ݒ�									*/
	VULONG	AREG_DIVSET;			/* 0x0164 �����@�\�ݒ�											*/
	VULONG	AREG_PLSET;				/* 0x0168 �p���X�ϊ��ݒ�										*/
	VULONG	AREG_WDT1SET;			/* 0x016C ���Z���pWDT�ݒ�										*/
	VULONG	AREG_POSET0;			/* 0x0170 �V���A��PG-�p���X�ϊ��ʒu�ݒ�							*/
	VULONG	AREG_POSET1;			/* 0x0174 �V���A��PG-�p���X�ϊ����_�␳1						*/
	VULONG	AREG_POSET2;			/* 0x0178 �V���A��PG-�p���X�ϊ����_�␳2						*/
	VULONG	AREG_POTEST;			/* 0x017C �V���A��PG-�p���X�ϊ��ݒ�L(�e�X�g�p)					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_POSET0H;			/* 0x0180 �V���A��PG-�p���X�ϊ��ʒu�ݒ�bit16-23					*/
	VULONG	SPARE12;				/* 0x0184 Reserve												*/
	VULONG	AREG_POSET12H;			/* 0x0188 �V���A��PG-�p���X�ϊ����_�␳H						*/
	VULONG	AREG_WDT2SET;			/* 0x018C �z�X�g�pWDT�ݒ�										*/
	VULONG	AREG_DCTRIM;			/* 0x0190 �����o�͌��_�␳�ݒ�(Bit0-15)							*/
	VULONG	AREG_ADSET1;			/* 0x0194 AD�ݒ�												*/
	VULONG	AREG_ADSET2;			/* 0x0198 �w��AD�ݒ�F��{�N���b�N								*/
	VULONG	AREG_ADSET3;			/* 0x019C �w��AD�ݒ�FIF�T�C�N��								*/
	VULONG	AREG_ADSET4;			/* 0x01A0 �w��AD�ݒ�F���b�`�^�C�~���O							*/
	VULONG	AREG_ADSET5;			/* 0x01A4 �w��AD�ݒ�F�R���g���[��CH0							*/
	VULONG	AREG_ADSET6;			/* 0x01A8 �w��AD�ݒ�F�R���g���[��CH1							*/
	VULONG	AREG_ADSYNC;			/* 0x01AC �w��AD�T�C�N������									*/
	VULONG	AREG_ADFCNT1;			/* 0x01B0 �d��AD�ݒ�F��{�N���b�N								*/
	VULONG	AREG_ADFCNT2;			/* 0x01B4 �d��AD�ݒ�FIF�T�C�N��								*/
	VULONG	AREG_ADFCNT3;			/* 0x01B8 �d��AD�ݒ�F���b�`�^�C�~���O							*/
	VULONG	AREG_ADFCNT4;			/* 0x01BC �d��AD�ݒ�F�R���g���[��CH0							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_ADFCNT5;			/* 0x01C0 �d��AD�ݒ�F�R���g���[��CH1							*/
	VULONG	AREG_ADFSYNC;			/* 0x01C4 �d��AD�T�C�N������									*/
	VULONG	AREG_INT0SET;			/* 0x01C8 INT0�����ݐݒ�										*/
	VULONG	AREG_ACULH;				/* 0x01CC �w��AD�FCH0���b�`�f�[�^(Bit16-23)						*/
	VULONG	AREG_INT0TRG;			/* 0x01D0 INT0�����ݓ����|�[�g/�J�E���^�l�Ǐo��					*/
	VULONG	AREG_DCNTV;				/* 0x01D4 �����J�E���^�ݒ�l(Bit0-15)							*/
	VULONG	AREG_ACUL;				/* 0x01D8 �w��AD�FCH0���b�`�f�[�^								*/
	VULONG	AREG_ACVL;				/* 0x01DC �w��AD�FCH1���b�`�f�[�^(Bit0-15)						*/
	VULONG	AREG_AFUL;				/* 0x01E0 �d��AD�FU���d�����b�`�f�[�^							*/
	VULONG	AREG_AFVL;				/* 0x01E4 �d��AD�FV���d�����b�`�f�[�^							*/
	VULONG	AREG_CMDAD1;			/* 0x01E8 �w��AD�f�[�^1(�e�X�g�p)								*/
	VULONG	AREG_CMDAD2;			/* 0x01EC �w��AD�f�[�^2(�e�X�g�p)								*/
	VULONG	AREG_CURAD1;			/* 0x01F0 �d��AD�f�[�^1(�e�X�g�p)								*/
	VULONG	AREG_CURAD2;			/* 0x01F4 �d��AD�f�[�^2(�e�X�g�p)								*/
	VULONG	AREG_DDACT;				/* 0x01F8 �V���A��PG-�p���X�ϊ�DDA�l(�e�X�g�p)					*/
	VULONG	AREG_ACVLH;				/* 0x01FC �w��AD�FCH1���b�`�f�[�^(Bit16-23)						*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_STIMLT5;			/* 0x0200 ���b�`�^�C�}���b�`�l1d								*/
	VULONG	AREG_STIMLT6;			/* 0x0204 ���b�`�^�C�}���b�`�l2d								*/
	VULONG	AREG_STIMLT7;			/* 0x0208 ���b�`�^�C�}���b�`�l3d								*/
	VULONG	AREG_STIMLT8;			/* 0x020C �G���R�[�_�Ǎ��ݎ��̃^�C�}���b�`�l					*/
	VULONG	AREG_SFBLT0;			/* 0x0210 CA�[�q�����オ�莞�̃t�B�[�h�o�b�N�ʒu				*/
	VULONG	AREG_SFBLT1;			/* 0x0214 CA�[�q���������莞�̃t�B�[�h�o�b�N�ʒu				*/
	VULONG	AREG_SFBLT2;			/* 0x0218 CB�[�q�����オ�莞�̃t�B�[�h�o�b�N�ʒu				*/
	VULONG	AREG_SFBLT3;			/* 0x021C CB�[�q���������莞�̃t�B�[�h�o�b�N�ʒu				*/
	VULONG	AREG_SFBLT4;			/* 0x0220 CC�[�q�����オ�莞�̃t�B�[�h�o�b�N�ʒu				*/
	VULONG	AREG_SFBLT5;			/* 0x0224 CC�[�q���������莞�̃t�B�[�h�o�b�N�ʒu				*/
	VULONG	SPARE13;				/* 0x0228 Reserve												*/
	VULONG	AREG_OCDCONF1;			/* 0x022C OC���o�f�V���[�V�����t�B���^���샂�[�h�ݒ�			*/
	VULONG	AREG_OCLVUP;			/* 0x0230 U�����OC���o���x��									*/
	VULONG	AREG_TIM;				/* 0x0234 ���b�`�^�C�}���ݒl									*/
	VULONG	AREG_OCLVUM;			/* 0x0238 U������OC���o���x��									*/
	VULONG	AREG_OCLVVP;			/* 0x023C V�����OC���o���x��									*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_NCTCSET;			/* 0x0240 �w�ߗpNCT�ݒ�											*/
	VULONG	AREG_AFULH;				/* 0x0244 �d��AD�FCH0���b�`�f�[�^(Bit16-23)						*/
	VULONG	AREG_AFVLH;				/* 0x0248 �d��AD�FCH1���b�`�f�[�^(Bit16-23)						*/
	VULONG	AREG_ACMC4;				/* 0x024C �w�߃}���`�v���N�T�`���l��4							*/
	VULONG	AREG_NCTCTM;			/* 0x0250 �w�ߗpNCT�F�^�C�}�J�E���^�l							*/
	VULONG	AREG_NCTCFB;			/* 0x0254 �w�ߗpNCT�FFB�J�E���^�l								*/
	VULONG	AREG_NCTCCD;			/* 0x0258 �w�ߗpNCT�FCD�J�E���^�l								*/
	VULONG	AREG_NCTCTP;			/* 0x025C �w�ߗpNCT�FTP�J�E���^�l								*/
	VULONG	AREG_POTESTH;			/* 0x0260 �V���A��PG-�p���X�ϊ��ݒ�H(�e�X�g�p)					*/
	VULONG	AREG_HINT1;				/* 0x0264 ���Z�������݃|�[�g1									*/
	VULONG	AREG_HINT2;				/* 0x0268 ���Z�������݃|�[�g2									*/
	VULONG	AREG_ACMC5;				/* 0x026C �w�߃}���`�v���N�T�`���l��5							*/
	VULONG	AREG_SRESET;			/* 0x0270 �\�t�g���Z�b�g										*/
	VULONG	AREG_NCTRSTL;			/* 0x0274 NCT���Z�b�g											*/
	VULONG	AREG_ACMC6;				/* 0x0278 �w�߃}���`�v���N�T�`���l��6							*/
	VULONG	AREG_ACMC7;				/* 0x027C �w�߃}���`�v���N�T�`���l��7							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_SE1_TXD01;			/* 0x0280 �V���A���G���R�[�_CH1���M�f�[�^0,1					*/
	VULONG	AREG_SE1_TXD23;			/* 0x0284 �V���A���G���R�[�_CH1���M�f�[�^2,3					*/
	VULONG	AREG_SE1_TXD45;			/* 0x0288 �V���A���G���R�[�_CH1���M�f�[�^4,5					*/
	VULONG	AREG_SE1_TXD67;			/* 0x028C �V���A���G���R�[�_CH1���M�f�[�^6,7					*/
	VULONG	AREG_SE1_RXD01;			/* 0x0290 �V���A���G���R�[�_CH1��M�f�[�^0,1					*/
	VULONG	AREG_SE1_RXD23;			/* 0x0294 �V���A���G���R�[�_CH1��M�f�[�^2,3					*/
	VULONG	AREG_SE1_RXD45;			/* 0x0298 �V���A���G���R�[�_CH1��M�f�[�^4,5					*/
	VULONG	AREG_SE1_RXD67;			/* 0x029C �V���A���G���R�[�_CH1��M�f�[�^6,7					*/
	VULONG	AREG_OCSDMCONF3;		/* 0x02A0 OC���o�ݒ�3											*/
	VULONG	AREG_OCSDMCONF4;		/* 0x02A4 OC���o�ݒ�4											*/
	VULONG	AREG_OCVADSYNC;			/* 0x02A8 OC���o�pAD�T�C�N�������i�f�[�^�C�Ӂj					*/
	VULONG	AREG_OCADU;				/* 0x02AC OC���o�pU��AD�f�[�^									*/
	VULONG	AREG_OCADV;				/* 0x02B0 OC���o�pV��AD�f�[�^									*/
	VULONG	AREG_OCADW;				/* 0x02B4 OC���o�pW��AD�f�[�^									*/
	VULONG	SPARE14[2];				/* 0x02B8-0x02BC Reserve										*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_STIMLT0;			/* 0x02C0 ���b�`�^�C�}���b�`�l0									*/
	VULONG	AREG_STIMLT1;			/* 0x02C4 ���b�`�^�C�}���b�`�l1									*/
	VULONG	AREG_STIMLT2;			/* 0x02C8 ���b�`�^�C�}���b�`�l2									*/
	VULONG	AREG_STIMLT3;			/* 0x02CC ���b�`�^�C�}���b�`�l3									*/
	VULONG	AREG_STIMLT4;			/* 0x02D0 ���b�`�^�C�}���b�`�l4									*/
	VULONG	AREG_STIMSTAT;			/* 0x02D4 ���b�`�^�C�}�ݒ�										*/
	VULONG	AREG_ENCOFS;			/* 0x02D8 �t�B�[�h�o�b�N�ʒu�␳�l(�����t)						*/
	VULONG	AREG_STCLRLT;			/* 0x02DC ���b�`�X�e�[�^�X�̃N���A����							*/
	VULONG	AREG_SADAT_A;			/* 0x02E0 �G���R�[�_AD�f�[�^A									*/
	VULONG	AREG_SADAT_AA;			/* 0x02E4 �G���R�[�_AD�f�[�^AA									*/
	VULONG	AREG_SADAT_B;			/* 0x02E8 �G���R�[�_AD�f�[�^B									*/
	VULONG	AREG_SADAT_BB;			/* 0x02EC �G���R�[�_AD�f�[�^BB									*/
	VULONG	AREG_EIFSET1;			/* 0x02F0 �G���R�[�_ADIF�ݒ�1									*/
	VULONG	AREG_EIFSET2;			/* 0x02F4 �G���R�[�_ADIF�ݒ�2									*/
	VULONG	AREG_SRCNT0;			/* 0x02F8 �V���A���G���R�[�_IF0�`�������ݒ�						*/
	VULONG	AREG_SRCNT1;			/* 0x02FC �V���A���G���R�[�_IF1�`�������ݒ�						*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_FBZRND;			/* 0x0300 �t�B�[�h�o�b�N�J�E���^ZRN���b�`�f�[�^					*/
	VULONG	AREG_FBCLT;				/* 0x0304 �t�B�[�h�o�b�N�J�E���^���[�h���b�`�f�[�^				*/
	VULONG	AREG_FBPUD;				/* 0x0308 �t�B�[�h�o�b�N�J�E���^PU���b�`�f�[�^					*/
	VULONG	AREG_FBPCD;				/* 0x030C �t�B�[�h�o�b�N�J�E���^PC���b�`�f�[�^					*/
	VULONG	AREG_TCNT;				/* 0x0310 �t�B�[�h�o�b�N�^�C�}���ݒl							*/
	VULONG	AREG_T1LT;				/* 0x0314 �t�B�[�h�o�b�N�^�C�}A/B�p���X���b�`�l					*/
	VULONG	AREG_T2LT;				/* 0x0318 �t�B�[�h�o�b�N�^�C�}�L�����A���b�`�l					*/
	VULONG	AREG_T3LT;				/* 0x031C �t�B�[�h�o�b�N�^�C�}T1���O�l���b�`�l					*/
	VULONG	AREG_FCCFZRNL;			/* 0x0320 �t���N���[�Y�h�J�E���^ZRN���b�`�f�[�^					*/
	VULONG	AREG_FCLDL;				/* 0x0324 �t���N���[�Y�h�J�E���^���[�h���b�`�f�[�^				*/
	VULONG	AREG_DCTRIMH;			/* 0x0328 �����o�͌��_�␳�ݒ�(Bit16-23)						*/
	VULONG	AREG_DCNTVH;			/* 0x032C �����J�E���^�ݒ�l(Bit16-23)							*/
	VULONG	AREG_OCLVVM;			/* 0x0330 V�����lOC���o���x��									*/
	VULONG	AREG_OCLVWP;			/* 0x0334 W�����lOC���o���x��									*/
	VULONG	AREG_OCLVWM;			/* 0x0338 W�����lOC���o���x��									*/
	VULONG	AREG_FLTRANGE;			/* 0x033C OC�A���[���t�B���^�񐔐ݒ�							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_SDMCONF3;			/* 0x0340 �f�V���[�V�����t�B���^�ݒ�3							*/
	VULONG	AREG_SDMCONF4;			/* 0x0344 �f�V���[�V�����t�B���^�ݒ�4							*/
	VULONG	AREG_VADSYNC;			/* 0x0348 �d�����o�pAD�T�C�N������(���W�X�^�������ݓ���)		*/
	VULONG	AREG_AVL;				/* 0x034C �d�����o�pAD���b�`�f�[�^								*/
	VULONG	AREG_VAD;				/* 0x0350 �d�����o�pAD�f�[�^									*/
	VULONG	SPARE15;				/* 0x0354 Reserve												*/
	VULONG	AREG_SETMSK;			/* 0x0358 �}�X�N�ݒ肨���O2CDAT								*/
	VULONG	AREG_DMASET;			/* 0x035C DMA�ݒ�												*/
	VULONG	AREG_ENCTXDLY0;			/* 0x0360 �G���R�[�_ch0���M�J�n�x�����Ԑݒ�						*/
	VULONG	AREG_ENCTXDLY1;			/* 0x0364 �G���R�[�_ch1���M�J�n�x�����Ԑݒ�						*/
	VULONG	AREG_ENCRDDLY0;			/* 0x0368 �G���R�[�_�ʒu�␳�J�nch0�x�����Ԑݒ�					*/
	VULONG	AREG_ENCRDDLY1;			/* 0x036C �G���R�[�_�ʒu�␳�J�nch1�x�����Ԑݒ�					*/
	VULONG	AREG_INTASET;			/* 0x0370 ScanA�����ݐݒ�										*/
	VULONG	AREG_INTBSET;			/* 0x0374 ScanB�����ݐݒ�										*/
	VULONG	AREG_INTCSET;			/* 0x0378 ScanC�����ݐݒ�										*/
	VULONG	AREG_INTNSET;			/* 0x037C ScanN�����ݐݒ�										*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_INTCYCLE;			/* 0x0380 �����ݎ����ݒ�										*/
	VULONG	AREG_INTMPESET;			/* 0x0384 �}�C�N���G���R�[�_���������ݐݒ�						*/
	VULONG	AREG_INTSET;			/* 0x0388 �����ݐݒ�											*/
	VULONG	SPARE16;				/* 0x038C Reserve												*/
	VULONG	AREG_ENCFLAG;			/* 0x0390 �G���R�[�_�␳�ݒ肨��уA���[��						*/
	VULONG	AREG_PGDCAL;			/* 0x0394 �ʒu�f�[�^�֘A�ݒ�									*/
	VULONG	AREG_PULSENO;			/* 0x0398 1��]�G���R�[�_�p���X���ݒ�							*/
	VULONG	SPARE17;				/* 0x039C Reserve												*/
	VULONG	AREG_ACCERRLVL;			/* 0x03A0 �����x�G���[���x���ݒ�								*/
	VULONG	SPARE18;				/* 0x03A4 Reserve												*/
	VULONG	AREG_SPGFAIL;			/* 0x03A8 �݌v�G���[���x���ݒ�									*/
	VULONG	AREG_LCHCLKNUM;			/* 0x03AC ���b�`�������N���b�N���ݒ�							*/
	VULONG	AREG_SACLKNUM;			/* 0x03B0 ���Z�������N���b�N���ݒ�								*/
	VULONG	AREG_BASTIM;			/* 0x03B4 �x�[�X�^�C�}�l���[�h���b�`�f�[�^						*/
	VULONG	AREG_LASTBASTIM;		/* 0x03B8 �O��x�[�X�^�C�}�l���[�h���b�`�f�[�^					*/
	VULONG	AREG_HENKAITITIM;		/* 0x03BC �ω��ʒu�^�C�}�l���[�h���b�`�f�[�^					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	SPARE19;				/* 0x03C0 Reserve												*/
	VULONG	AREG_COMERRCNT;			/* 0x03C4 �ʐM�G���[�J�E���^�l���[�h���b�`�f�[�^				*/
	VULONG	AREG_ACCERRCNT;			/* 0x03C8 �����x�G���[�J�E���^�l���[�h���b�`�f�[�^				*/
	VULONG	AREG_RSTSET;			/* 0x03CC ���Z�b�g/�Z�b�g�ݒ�									*/
	VULONG	AREG_SPGRXF;			/* 0x03D0 ��M�t���O�ݒ�										*/
	VULONG	AREG_SPGD12;			/* 0x03D4 �G���R�[�_��M�f�[�^�ݒ�								*/
	VULONG	AREG_SPGD34;			/* 0x03D8 �G���R�[�_��M�f�[�^�ݒ�								*/
	VULONG	AREG_SPGD5;				/* 0x03DC �G���R�[�_��M�f�[�^�ݒ�								*/
	VULONG	AREG_CURMOTPOS;			/* 0x03E0 �␳��ʒu�f�[�^ ���W�X�^���b�`�f�[�^					*/
	VULONG	AREG_LSTMOTPOS;			/* 0x03E4 �O��̕␳��ʒu�f�[�^ ���W�X�^���b�`�f�[�^			*/
	VULONG	AREG_CURRCVPOS;			/* 0x03E8 �␳�����ʒu�f�[�^ ���W�X�^���b�`�f�[�^				*/
	VULONG	AREG_LSTRCVPOS;			/* 0x03EC �O��̕␳�����ʒu�f�[�^ ���W�X�^���b�`�f�[�^			*/
	VULONG	AREG_CURRXPOS0_L;		/* 0x03F0 ��M�f�[�^ ���W�X�^���b�`�f�[�^ Lo					*/
	VULONG	AREG_CURRXPOS0_H;		/* 0x03F4 ��M�f�[�^ ���W�X�^���b�`�f�[�^ Hi					*/
	VULONG	AREG_LSTRXPOS0_L;		/* 0x03F8 �O��̎�M�f�[�^ ���W�X�^���b�`�f�[�^ Lo				*/
	VULONG	AREG_LSTRXPOS0_H;		/* 0x03FC �O��̎�M�f�[�^ ���W�X�^���b�`�f�[�^ Hi				*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	MREG_UPLDA;				/* 0x0400 ASIC MicroPrg Load Pointer							*/
	VULONG	MREG_UPLD;				/* 0x0404 ASIC MicroPrg Load Buffer								*/
	VULONG	MREG_CTSTW;				/* 0x0408 ����status(READ-ONLY)									*/
	VULONG	MREG_CTSTR;				/* 0x040C ����cmdport(WRITE-ONLY)								*/
	VULONG	MREG_HSUR;				/* 0x0410 �z�X�g�w�߃|�[�g(WRITE-ONLY)							*/
} ASIC_HW_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-086 SERVOIP �ʃv���O�������W�X�^�\���̒�`											*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Axis information register																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _AXIS_INFO {
	LONG	AxisNum;			/* 0xC0007000 : ���䎲��											*/
} AXIS_INFO;

/*--------------------------------------------------------------------------------------------------*/
/*		32 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ACRV {
	DWREG	IdIntgl;			/* 0xC0007004 : ACRd Integral										*/
	DWREG	IqIntgl;			/* 0xC0007008 : ACRq Integral										*/
	DWREG	VdFil;				/* 0xC000700C : ACRd Filter Output									*/
	DWREG	VqFil;				/* 0xC0007010 : ACRq Filter Output									*/
} ACRV;

/*--------------------------------------------------------------------------------------------------*/
/*		16 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _STSFLG {
	SHORT	BbSetW;				/* 0xC0007014 : soft BB set(BBSET)									*/
	SHORT	FltStsW;			/* 0xC0007016 : fault status(FLTIN)									*/
	SHORT	CtrlStsRW;			/* 0xC0007018 : controll flag/status(CTSTR,CTSTW)					*/
	SHORT	CNTSTS;				/* 0xC000701A : counter status(FBCSTS)								*/
	SHORT	FccStsMon;			/* 0xC000701C : full closed counter status(FCCST)					*/
	SHORT	IntglFlg;			/* 0xC000701E : �ϕ��O�a�t���O										*/
} STSFLG;

/*--------------------------------------------------------------------------------------------------*/
/*		voltage FF																					*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _VCMPV {
	SHORT	LdC;				/* 0xC0007020 : Ld  * omega											*/
	SHORT	LqC;				/* 0xC0007022 : Lq  * omega											*/
	SHORT	MagC;				/* 0xC0007024 : Phi * omega											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdOut;				/* 0xC0007026 : Vd output voltage									*/
	SHORT	VqOut;				/* 0xC0007028 : Vq output voltage									*/
	SHORT	VdComp;				/* 0xC000702A : Vd compensation data								*/
	SHORT	VqComp;				/* 0xC000702C : Vq compensation data								*/
	SHORT	VuOut;				/* 0xC000702E : vu output data (to PWM)								*/
	SHORT	VvOut;				/* 0xC0007030 : vv output data (to PWM)								*/
	SHORT	VwOut;				/* 0xC0007032 : vw output data (to PWM)								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdRef;				/* 0xC0007034 : vdref												*/
	SHORT	VqRef;				/* 0xC0007036 : vqref												*/
	SHORT	Vmax2;				/* 0xC0007038 : �d���w�߃x�N�g���ő�l(8192*1.27)					*/
	SHORT	V12;				/* 0xC000703A : �d���w�߃x�N�g��(��(VdOut^2+VqOut^2))				*/
} VCMPV;

/*--------------------------------------------------------------------------------------------------*/
/*		trigonometric function value																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _SINTBL {
	SHORT	SinT;				/* 0xC000703C : sin theta											*/
	SHORT	CosT;				/* 0xC000703E : cos theta											*/
	SHORT	SinT2;				/* 0xC0007040 : sin(theta + 2pi/3)									*/
	SHORT	CosT2;				/* 0xC0007042 : cos(theta + 2pi/3)									*/
	SHORT	SinT3;				/* 0xC0007044 : sin(theta - 2pi/3)									*/
	SHORT	CosT3;				/* 0xC0007046 : cos(theta - 2pi/3)									*/
} SINTBL;

/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ADSTOP {
	SHORT	ADRst;				/* 0xC0007048 : 													*/
	SHORT	ADERRCNT;			/* 0xC000704A : ASIC AD Error Counter								*/
	SHORT	IRIUP;				/* 0xC000704C : Previous IRIU										*/
	SHORT	IRIUCNT;			/* 0xC000704E : same IRIU counter									*/
	SHORT	IRIVP;				/* 0xC0007050 : Previous IRIV										*/
	SHORT	IRIVCNT;			/* 0xC0007052 : same IRIV counter									*/
} ADSTOP;

/*--------------------------------------------------------------------------------------------------*/
/*		CPU I/F Data																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ADINV {
	SHORT	IuOffsetIn;			/* 0xC0007054 : A/D transfer offset for iu							*/
	SHORT	IvOffsetIn;			/* 0xC0007056 : A/D transfer offset for iv							*/
	SHORT	KcuIn;				/* 0xC0007058 : A/D transfer gain for iu							*/
	SHORT	KcvIn;				/* 0xC000705A : A/D transfer gain for iv							*/
	SHORT	IdIn;				/* 0xC000705C : Id reference										*/
	SHORT	IqIn;				/* 0xC000705E : Iq reference										*/
	SHORT	PhaseHIn;			/* 0xC0007060 : PHA input											*/
	SHORT	VelIn;				/* 0xC0007062 : VEL input											*/
/*--------------------------------------------------------------------------------------------------*/
/*		voltage compensation																		*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdRefIn;			/* 0xC0007064 : vdref input											*/
	SHORT	VqRefIn;			/* 0xC0007066 : vqref input											*/
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	TLimPIn;			/* 0xC0007068 : +tlim												*/
	SHORT	TLimMIn;			/* 0xC000706A : -tlim												*/
} ADINV;

/*--------------------------------------------------------------------------------------------------*/
/*		static variable																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADP {
	SHORT	Kcu;				/* 0xC000706C : A/D transfer gain for iu							*/
	SHORT	Kcv;				/* 0xC000706E : A/D transfer gain for iv							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for over modulation																			*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kmod;				/* 0xC0007070 : Over modulation gain/offset							*/
	SHORT	Kvv;				/* 0xC0007072 : AVR													*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kc;					/* 0xC0007074 : Current conversion Gain								*/
	SHORT	Ld;					/* 0xC0007076 : d axis Inductance									*/
	SHORT	Lq;					/* 0xC0007078 : q axis Inductance									*/
	SHORT	Mag;				/* 0xC000707A : Magnetic flux (Phi)									*/
	SHORT	KdP;				/* 0xC000707C : d axis propotion gain (PI control)					*/
	SHORT	KqP;				/* 0xC000707E : q axis propotion gain (PI control)					*/
	SHORT	KdI;				/* 0xC0007080 : d axis integral time (gain)							*/
	SHORT	KqI;				/* 0xC0007082 : q axis integral time (gain)							*/
	SHORT	VdLim;				/* 0xC0007084 : d axis integral limit								*/
	SHORT	VqLim;				/* 0xC0007086 : q axis integral limit								*/
	SHORT	KvvIn;				/* 0xC0007088 : Voltage Compasation gain							*/
	SHORT	OnDelayLvl;			/* 0xC000708A : On delay change level								*/
	SHORT	Tfil;				/* 0xC000708C : Filter time constant								*/
	SHORT	Vmax;				/* 0xC000708E : voltage limit data (Vmax^2)							*/
	SHORT	OnDelayComp;		/* 0xC0007090 : On-delay compensation								*/
	SHORT	CtrlSw;				/* 0xC0007092 : Control Flag										*/
	USHORT	CrFreq;				/* 0xC0007094 : carrier freq reference(change while BB)				*/
	SHORT	CtrlSw2;			/* 0xC0007096 : Control Flag2										*/
	SHORT	Dummy0;				/* 0xC0007098 : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch1																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf11;				/* 0xC000709A : 													*/
	SHORT	Kf12;				/* 0xC000709C : 													*/
	SHORT	Kf13;				/* 0xC000709E : 													*/
	SHORT	Kf14;				/* 0xC00070A0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch2																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf21;				/* 0xC00070A2 : 													*/
	SHORT	Kf22;				/* 0xC00070A4 : 													*/
	SHORT	Kf23;				/* 0xC00070A6 : 													*/
	SHORT	Kf24;				/* 0xC00070A8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch3																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf31;				/* 0xC00070AA : 													*/
	SHORT	Kf32;				/* 0xC00070AC : 													*/
	SHORT	Kf33;				/* 0xC00070AE : 													*/
	SHORT	Kf34;				/* 0xC00070B0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch4																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf41;				/* 0xC00070B2 : 													*/
	SHORT	Kf42;				/* 0xC00070B4 : 													*/
	SHORT	Kf43;				/* 0xC00070B6 : 													*/
	SHORT	Kf44;				/* 0xC00070B8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch5																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf51;				/* 0xC00070BA : 													*/
	SHORT	Kf52;				/* 0xC00070BC : 													*/
	SHORT	Kf53;				/* 0xC00070BE : 													*/
	SHORT	Kf54;				/* 0xC00070C0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch6																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf61;				/* 0xC00070C2 : 													*/
	SHORT	Kf62;				/* 0xC00070C4 : 													*/
	SHORT	Kf63;				/* 0xC00070C6 : 													*/
	SHORT	Kf64;				/* 0xC00070C8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		LPF																							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	TLpf;				/* 0xC00070CA : LPF1 gain											*/
	SHORT	TLpf2;				/* 0xC00070CC : LPF2 gain											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	MotResist;			/* 0xC00070CE : Moter resistance(r1/I_BASE*V_BASE)					*/
	SHORT	OnDelaySlope;		/* 0xC00070D0 : Dead-time comp. gain								*/
	SHORT	L_dIdt;				/* 0xC00070D2 : 													*/
	SHORT	FccRst;				/* 0xC00070D4 : FCCST Reset(for TEST)								*/
	SHORT	Dummy;				/* 0xC00070D6 : for Alignment										*/
} INTADP;

/*--------------------------------------------------------------------------------------------------*/
/*		Current Loop Control Data																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADV {
	SHORT	IuOffset;			/* 0xC00070D8 : A/D transfer offset for iu							*/
	SHORT	IvOffset;			/* 0xC00070DA : A/D transfer offset for iv							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IuInData;			/* 0xC00070DC : iu input data (from A/D)							*/
	SHORT	IvInData;			/* 0xC00070DE : iv input data (from A/D)							*/
	SHORT	IdInData;			/* 0xC00070E0 : Id Input											*/
	SHORT	IqInData;			/* 0xC00070E2 : Iq Input											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CrFreqW;			/* 0xC00070E4 : Carrier freq Now									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IuOut;				/* 0xC00070E6 : Iu reference(for dead-time cmp)						*/
	SHORT	IvOut;				/* 0xC00070E8 : Iv reference(for dead-time cmp)						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	V1;					/* 0xC00070EA : V1( = SQRT(VdOut^2+VqOut^2) )						*/
	SHORT	Vcent;				/* 0xC00070EC : Central voltage										*/
	SHORT	Dummy1;				/* 0xC00070EE : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		for LPF																						*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut1Lpf;			/* 0xC00070F0 : LPF1 Output(Low)									*/
	DWREG	IqOut2Lpf;			/* 0xC00070F4 : LPF2 Output(Low)									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqRef;				/* 0xC00070F8 : Iq Reference after limit							*/
	SHORT	TLimP;				/* 0xC00070FA : +tlim												*/
	SHORT	TLimM;				/* 0xC00070FC : -tlim												*/
/*--------------------------------------------------------------------------------------------------*/
/*		for monitor																					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqMon;				/* 0xC00070FE : IQ monitor											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IdDataP;			/* 0xC0007100 : 													*/
	SHORT	IqDataP;			/* 0xC0007102 : 													*/
	SHORT	KEangle;			/* 0xC0007104 : �@�B�p���d�C�p�ϊ��W��								*/
	SHORT	Dummy2;				/* 0xC0007106 : for Alignment										*/
	DWREG	IdLfil;				/* 0xC0007108 : L(di/dt)�t�B���^									*/
	DWREG	IqLfil;				/* 0xC000710C : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		for axis q monitor																			*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqDistIn;			/* 0xC0007110 : q���O���g���N����									*/
	SHORT	IqDist;				/* 0xC0007112 : q���O���g���N										*/
	SHORT	IqMonFil;			/* 0xC0007114 : q���d���w�߃��j�^(�t�B���^��)						*/
	SHORT	IqOfRef;			/* 0xC0007116 : q���d���w��(�O���g���N���Z��)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut1L;			/* 0xC0007118 : filter1 output(Low)									*/
	DWREG	IqOut1PL;			/* 0xC000711C : filter1 output 1delay(Low)							*/
	DWREG	IqOut1PPL;			/* 0xC0007120 : filter1 output 2delay(Low)							*/
	DWREG	IqIn1PL;			/* 0xC0007124 : filter1 input 1delay(Low)							*/
	DWREG	IqIn1PPL;			/* 0xC0007128 : filter1 input 2delay(Low)							*/
	SHORT	IQO1;				/* 0xC000712C : filter1 output(short type)							*/
	SHORT	Dummy3;				/* 0xC000712E : for Alignment										*/
	DWREG	IqOut1BufL;			/* 0xC0007130 : filter1 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut2L;			/* 0xC0007134 : filter2 output(Low)									*/
	DWREG	IqOut2PL;			/* 0xC0007138 : filter2 output 1delay(Low)							*/
	DWREG	IqOut2PPL;			/* 0xC000713C : filter2 output 2delay(Low)							*/
	DWREG	IqIn2PL;			/* 0xC0007140 : filter2 input 1delay(Low)							*/
	DWREG	IqIn2PPL;			/* 0xC0007144 : filter2 input 2delay(Low)							*/
	SHORT	IQO2;				/* 0xC0007148 : filter2 output(short type)							*/
	SHORT	Dummy4;				/* 0xC000714A : for Alignment										*/
	DWREG	IqOut2BufL;			/* 0xC000714C : filter2 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter3																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut3L;			/* 0xC0007150 : filter3 output(Low)									*/
	DWREG	IqOut3PL;			/* 0xC0007154 : filter3 output 1delay(Low)							*/
	DWREG	IqOut3PPL;			/* 0xC0007158 : filter3 output 2delay(Low)							*/
	DWREG	IqIn3PL;			/* 0xC000715C : filter3 input 1delay(Low)							*/
	DWREG	IqIn3PPL;			/* 0xC0007160 : filter3 input 2delay(Low)							*/
	SHORT	IqOut3;				/* 0xC0007164 : filter3 output(short type)							*/
	SHORT	Dummy5;				/* 0xC0007166 : for Alignment										*/
	DWREG	IqOut3BufL;			/* 0xC0007168 : filter3 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter4																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut4L;			/* 0xC000716C : filter4 output(Low)									*/
	DWREG	IqOut4PL;			/* 0xC0007170 : filter4 output 1delay(Low)							*/
	DWREG	IqOut4PPL;			/* 0xC0007174 : filter4 output 2delay(Low)							*/
	DWREG	IqIn4PL;			/* 0xC0007178 : filter4 input 1delay(Low)							*/
	DWREG	IqIn4PPL;			/* 0xC000717C : filter4 input 2delay(Low)							*/
	SHORT	IQO4;				/* 0xC0007180 : filter4 output(short type)							*/
	SHORT	Dummy6;				/* 0xC0007182 : for Alignment										*/
	DWREG	IqOut4BufL;			/* 0xC0007184 : filter4 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter5																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut5L;			/* 0xC0007188 : filter5 output(Low)									*/
	DWREG	IqOut5PL;			/* 0xC000718C : filter5 output 1delay(Low)							*/
	DWREG	IqOut5PPL;			/* 0xC0007190 : filter5 output 2delay(Low)							*/
	DWREG	IqIn5PL;			/* 0xC0007194 : filter5 input 1delay(Low)							*/
	DWREG	IqIn5PPL;			/* 0xC0007198 : filter5 input 2delay(Low)							*/
	SHORT	IQO5;				/* 0xC000719C : filter5 output(short type)							*/
	SHORT	Dummy7;				/* 0xC000719E : for Alignment										*/
	DWREG	IqOut5BufL;			/* 0xC00071A0 : filter5 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter6																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut6L;			/* 0xC00071A4 : filter6 output(Low)									*/
	DWREG	IqOut6PL;			/* 0xC00071A8 : filter6 output 1delay(Low)							*/
	DWREG	IqOut6PPL;			/* 0xC00071AC : filter6 output 2delay(Low)							*/
	DWREG	IqIn6PL;			/* 0xC00071B0 : filter6 input 1delay(Low)							*/
	DWREG	IqIn6PPL;			/* 0xC00071B4 : filter6 input 2delay(Low)							*/
	SHORT	IQO6;				/* 0xC00071B8 : filter6 output(short type)							*/
	SHORT	Dummy8;				/* 0xC00071BA : for Alignment										*/
	DWREG	IqOut6BufL;			/* 0xC00071BC : filter6 output buffer(Low)							*/
} INTADV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DOBSP {
	SHORT	TsPerL;				/* 0xC00071C0 : Ts/L ( �I�u�U�[�o�p�p�����[�^ )						*/
	SHORT	Gobs;				/* 0xC00071C2 : g ( �I�u�U�[�o�̋� )								*/
	SHORT	RLTs;				/* 0xC00071C4 : 1-R�Ts/L ( �I�u�U�[�o�p�p�����[�^ )					*/
	SHORT	FilObsGain;			/* 0xC00071C6 : �t�B���^�Q�C��										*/
/*--------------------------------------------------------------------------------------------------*/
} DOBSP;

/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DOBSV {
	DWREG	LpfIlq;				/* 0xC00071C8 : q�����[�p�X�t�B���^�ϐ�								*/
	DWREG	HpfIlq;				/* 0xC00071CC : q���n�C�p�X�t�B���^�ϐ�								*/
	SHORT	IqObsOut;			/* 0xC00071D0 : q���I�u�U�[�o�o��									*/
	SHORT	IqObsFreq;			/* 0xC00071D2 : q���U������											*/
	DWREG	LpfIld;				/* 0xC00071D4 : d�����[�p�X�t�B���^�ϐ�								*/
	DWREG	HpfIld;				/* 0xC00071D8 : d���n�C�p�X�t�B���^�ϐ�								*/
	SHORT	IdObsOut;			/* 0xC00071DC : d���I�u�U�[�o�o��									*/
	SHORT	IdObsFreq;			/* 0xC00071DE : d���U������											*/
	SHORT	DmpGain;			/* 0xC00071E0 : �d���I�u�U�[�o�_���s���O�Q�C��						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC00071E2 : for Alignment										*/
} DOBSV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Phase Interpolate																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _PHASEV {
	SHORT	PHAL;				/* 0xC00071E4 : Phase(Low)											*/
	SHORT	PhaseH;				/* 0xC00071E6 : Phase(High)											*/
	SHORT	PhaseIp;			/* 0xC00071E8 : �ʑ���ԗ�											*/
	SHORT	PhaseIpF;			/* 0xC00071EA : �ʑ���ԃt���O(0�F���Ȃ��A1�F����)					*/
	SHORT	PhaseIpIn;			/* 0xC00071EC : �ʑ���ԗ�(CPU��ASIC�󂯓n���p)						*/
	SHORT	PhaseIpFIn;			/* 0xC00071EE : �ʑ���ԃt���O(CPU��ASIC�󂯓n���p)					*/
} PHASEV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Encoder IF																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ENCIFV {
	SHORT	AmpType;			/* 0xC00071F0 : Rotary Type or Linear Type							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC00071F2 : for Alignment										*/
	DWREG	RcvPosX0;			/* 0xC00071F4 : Motor Encoder Position(��M�ʒu)					*/
	DWREG	RcvPosX1;			/* 0xC00071F8 : Motor Encoder Position(��M�ʒu)					*/
	DWREG	RcvPosX2;			/* 0xC00071FC : Motor Encoder Position(��M�ʒu)					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	MotPosSftR;			/* 0xC0007200 : Shift Data for Motor Position Cal.					*/
	SHORT	MotPosSftX;			/* 0xC0007202 : Shift Data for Motor Position Cal.					*/
	SHORT	MotPosSign;			/* 0xC0007204 : Sign  Data for Motor Position Cal.					*/
	SHORT	DivOutSft;			/* 0xC0007206 : �����o�̓V�t�g��									*/
	DWREG	AccErrLv;			/* 0xC0007208 : Motor Encoder Acc. Error Check Low					*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	DivPls;				/* 0xC000720C : �����o�̓p���X										*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	DivOutGain;			/* 0xC0007210 : �����o�̓Q�C��(���j�A)								*/
	DWREG	DivPos;				/* 0xC0007214 : �����o�̓p���X(���j�A)								*/
	DWREG	DivPlsRem;			/* 0xC0007218 : �����o�̓p���X�]��(���j�A)							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	SPGFail;			/* 0xC000721C : S-PG��M���s���e��								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	BitIprm;			/* 0xC000721E : Bit Initial Parameter								*/
	SHORT	BitData;			/* 0xC0007220 : Bit Data											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IncPlsReq;			/* 0xC0007222 : PAO IncrePulse Output Request						*/
	SHORT	PAOSeqCmd;			/* 0xC0007224 : PAO Serial Output Sequence							*/
	SHORT	PlsOSetCmd;			/* 0xC0007226 : �p���X�o�͉�H�������v��							*/
} ENCIFV;

/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊֘A�FHostCPU --> Asic ������]���p											*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DIVPLSV {
	SHORT	IncPlsReqIn;		/* 0xC0007228 : PAO IncrePulse Output Request						*/
	SHORT	PAOSeqCmdIn;		/* 0xC000722A : PAO Serial Output Sequence							*/
	SHORT	PlsOSetCmdIn;		/* 0xC000722C : �p���X�o�͉�H�������v��							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	DivSetIn;			/* 0xC000722E : �����@�\�ݒ����									*/
	SHORT	PoSet1In;			/* 0xC0007230 : �p���X�ϊ����_�␳1									*/
	SHORT	PoSet2In;			/* 0xC0007232 : �p���X�ϊ����_�␳2									*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IncInitPlsIn;		/* 0xC0007234 : �����C���N���p���X									*/
	DWREG	IncInitRemIn;		/* 0xC0007238 : �����C���N���p���X�]��								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	AccCntClrReq;		/* 0xC000723C : �����x�`�F�b�N�J�n�J�E���g�N���A�v��				*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC000723E : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Register for Library																		*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	Argu0;				/* 0xC0007240 : Argument0											*/
	DWREG	Argu1;				/* 0xC0007244 : Argument1											*/
	SHORT	Argu2;				/* 0xC0007248 : Argument2											*/
	SHORT	Argu2H;				/* 0xC000724A : Argument2 High Word									*/
	DWREG	Ret0;				/* 0xC000724C : Return Register										*/
	DWREG	Kx;					/* 0xC0007250 : kx													*/
	DWREG	Sx;					/* 0xC0007254 : sx													*/
	DWREG	Iu0;				/* 0xC0007258 : iu[0]												*/
	SHORT	Iu1;				/* 0xC000725C : iu[1]												*/
	SHORT	Iu1H;				/* 0xC000725E : iu[1] High Word										*/
} DIVPLSV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Weaken Field																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _WEAKFV {
	SHORT	WfKpVLIn;			/* 0xC0007260 : �d��FB���Q�C��(����16bit)							*/
	SHORT	WfKpVHIn;			/* 0xC0007262 : �d��FB���Q�C��(���16bit)							*/
	SHORT	WfKiVLIn;			/* 0xC0007264 : �d��FB�ϕ��Q�C��(����16bit)							*/
	SHORT	WfKiVHIn;			/* 0xC0007266 : �d��FB�ϕ��Q�C��(���16bit)							*/
	SHORT	WfV1MaxIn;			/* 0xC0007268 : �d���w�ߐ������x��									*/
	SHORT	WfIdRefLimIn;		/* 0xC000726A : d���d���w�߃��~�b�g									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	WfIntegLim;			/* 0xC000726C : �d��FB�ϕ����~�b�g									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IdOut;				/* 0xC000726E : Id reference										*/
	SHORT	IqOut;				/* 0xC0007270 : Iq reference										*/
	SHORT	Vel;				/* 0xC0007272 : Velocity (omega)									*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	WfKpV;				/* 0xC0007274 : �d��FB���Q�C��(����16bit)							*/
	DWREG	WfKiV;				/* 0xC0007278 : �d��FB�ϕ��Q�C��(����16bit)							*/
	SHORT	WfV1Max;			/* 0xC000727C : �d���w�ߐ������x��									*/
	SHORT	WfIdRefLim;			/* 0xC000727E : d���d���w�߃��~�b�g									*/
	SHORT	WfVqMax;			/* 0xC0007280 : q���d���w�߃��~�b�g									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC0007282 : for Alignment										*/
	DWREG	WfIntgl;			/* 0xC0007284 : �d��FB�ϕ��l										*/
	SHORT	WfVdRef;			/* 0xC0007288 : d���d���w��											*/
	SHORT	WfVqRef;			/* 0xC000728A : q���d���w��											*/
} WEAKFV;

/*--------------------------------------------------------------------------------------------------*/
/*		for make PWM Carrer Counter																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _PWMV {
	SHORT	PwmCntT2;			/* 0xC000728C : PWM�L�����A�J�E���^T2								*/
	SHORT	PwmCntT1;			/* 0xC000728E : PWM�L�����A�J�E���^T1								*/
	SHORT	PwmCntT0;			/* 0xC0007290 : PWM�L�����A�J�E���^T0								*/
	SHORT	Dummy;				/* 0xC0007292 : for Alignment										*/
} PWMV;

/*--------------------------------------------------------------------------------------------------*/
/*		Version define register																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _VER_INFO {
	SHORT	MswVer;				/* 0xC0007294 : �\�t�g�o�[�W�������								*/
	SHORT	TstVer;				/* 0xC0007296 : �e�X�g�o�[�W�������								*/
	SHORT	YspVer;				/* 0xC0007298 : �x�d�l�o�[�W�������								*/
	SHORT	Dummy;				/* 0xC000729A : for Alignment										*/
} VER_INFO;

/*--------------------------------------------------------------------------------------------------*/
/*		Common work register																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _COMWORKS {
	SHORT	WREG82;				/* 0xC000729C : Work  Register 82									*/
	SHORT	WREG83;				/* 0xC000729E : Work  Register 83									*/
	SHORT	WREG84;				/* 0xC00072A0 : Work  Register 84									*/
	SHORT	WREG85;				/* 0xC00072A2 : Work  Register 85									*/
	SHORT	WREG86;				/* 0xC00072A4 : Work  Register 86									*/
	SHORT	WREG87;				/* 0xC00072A6 : Work  Register 87									*/
	SHORT	WREG88;				/* 0xC00072A8 : Work  Register 88									*/
	SHORT	WREG89;				/* 0xC00072AA : Work  Register 89									*/
	SHORT	WREG95;				/* 0xC00072AC : Work  Register 95									*/
	SHORT	WREG100;			/* 0xC00072AE : Work  Register 100									*/
	SHORT	WREG101;			/* 0xC00072B0 : Work  Register 101									*/
	SHORT	WREG102;			/* 0xC00072B2 : Work  Register 102									*/
	SHORT	WREG103;			/* 0xC00072B4 : Work  Register 103									*/
	SHORT	WREG104;			/* 0xC00072B6 : Work  Register 104									*/
	SHORT	WREG109;			/* 0xC00072B8 : Work  Register 109									*/
	SHORT	Dummy;				/* 0xC00072BA : for Alignment										*/
} COMWORKS;

typedef struct _HOST_WOKS {
	DWREG	HTMP0;				/* 0xC00072BC : HOST Interrupt Temp0								*/
	DWREG	HTMP2;				/* 0xC00072C0 : HOST Interrupt Temp2								*/
	DWREG	HTMP4;				/* 0xC00072C4 : HOST Interrupt Temp4								*/
	DWREG	HTMP6;				/* 0xC00072C8 : HOST Interrupt Temp6								*/
	SHORT	HTMP7;				/* 0xC00072CC : HOST Interrupt Temp7								*/
	SHORT	HTMP8;				/* 0xC00072CE : HOST Interrupt Temp8								*/
} HOST_WOKS;

typedef struct _ROUND_WORKS {
	SHORT	WREG140;			/* 0xC00072D0 : Work Register 140									*/
	SHORT	WREG141;			/* 0xC00072D2 : Work Register 141									*/
	SHORT	WREG142;			/* 0xC00072D4 : Work Register 142									*/
	SHORT	WREG143;			/* 0xC00072D6 : Work Register 143									*/
	SHORT	WREG144;			/* 0xC00072D8 : Work Register 144									*/
	SHORT	WREG145;			/* 0xC00072DA : Work Register 145									*/
	SHORT	WREG146;			/* 0xC00072DC : Work Register 146									*/
	SHORT	WREG147;			/* 0xC00072DE : Work Register 147									*/
	SHORT	WREG148;			/* 0xC00072E0 : Work Register 148									*/
	SHORT	WREG149;			/* 0xC00072E2 : Work Register 149									*/
} ROUND_WORKS;

typedef struct _AD_WORKS {
	DWREG	TMP0;				/* 0xC00072E4 : AD Interrupt Temp0									*/
	DWREG	TMP2;				/* 0xC00072E8 : AD Interrupt Temp2									*/
	DWREG	TMP4;				/* 0xC00072EC : AD Interrupt Temp4									*/
	DWREG	TMP6;				/* 0xC00072F0 : AD Interrupt Temp6									*/
	DWREG	TMP8;				/* 0xC00072F4 : AD Interrupt Temp8									*/
} AD_WORKS;

/*--------------------------------------------------------------------------------------------------*/
/*		�����������p���[�N																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INITWK {
	SHORT	IN_WK0;				/* 0xC00072F8 : �����������p Work0 Lo								*/
	SHORT	IN_WK0H;			/* 0xC00072FA : �����������p Work0 Hi								*/
	SHORT	IN_WK1;				/* 0xC00072FC : �����������p Work1 Lo								*/
	SHORT	IN_WK1H;			/* 0xC00072FE : �����������p Work1 Hi								*/
} INITWK;




/****************************************************************************************************/
/*		Axis Control Structure definition															*/
/****************************************************************************************************/
typedef struct _ASIC_U_IF_STR {
	AXIS_INFO		AxisInfo;	/* �����ϐ�														*/
	ACRV			AcrV;		/* �d������p�ϐ�													*/
	STSFLG			StsFlg;		/* �X�e�[�^�X�t���O													*/
	VCMPV			VcmpV;		/* �d���⏞���Z�p�ϐ�												*/
	SINTBL			SinTbl;		/* sin�e�[�u��														*/
	ADSTOP			AdStop;		/* �d�������~�G���[���o�p�\����									*/
	ADINV			AdinV;		/* �z�X�gCPU�󂯓n���ϐ�											*/
	INTADP			IntAdP;		/* �d�������݃p�����[�^												*/
	INTADV			IntAdV;		/* �d�������ݕϐ�													*/
	DOBSP			DobsP;		/* d���I�u�U�[�o�p�p�����[�^										*/
	DOBSV			DobsV;		/* d���I�u�U�[�o�p�ϐ�												*/
	PHASEV			PhaseV;		/* �ʑ����Z�p�ϐ�													*/
	ENCIFV			EncIfV;		/* �G���R�[�_�C���^�t�F�[�X�ϐ�										*/
	DIVPLSV			DivPlsV;	/* �����p���X�p�ϐ�													*/
	WEAKFV			WeakFV;		/* ��ߊE���p�ϐ�													*/
	PWMV			PwmV;		/* PWM�L�����A�J�E���^�i�[�ϐ�										*/
	USHORT			CtrlStsIn;	/* �ʃv���O��������X�e�[�^�X(CPU�˃ʃv���O����)					*/
	USHORT			CtrlStsOut;	/* �ʃv���O��������X�e�[�^�X(�ʃv���O������CPU)					*/
	SHORT			Dummy[305];	/* Dummy buffer for Axis offset										*/
} ASIC_U_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		CSW (Control Select Switch )																*/
/****************************************************************************************************/
typedef struct _MICRO_CSW
{
	USHORT	data;
} MICRO_CSW;
#define ICLR		0x0001			/* bit.0  �ϕ����~�b�g�������F�z�[���h(0)�^�N���A(1)			*/
#define ISEL		0x0002			/* bit.1  Ld/Lq�⏞�F�d��FB(0)�^�d���w��(1)						*/
//;--				0x0004			/* bit.2  --Reserve--											*/
#define OBSSEL		0x0008			/* bit.3  Current Observer Select bit							*/
#define F1DSABL	0x0010			/* bit.4  Notch filter1 Disable									*/
#define F2DSABL	0x0020			/* bit.5  Notch filter2 Disable									*/
#define LPFDSABL	0x0040			/* bit.6  Low Pass filter1 Disable								*/
#define LPFCDSABL	0x0080			/* bit.7  Low Pass filter2 Disable								*/
#define OVMSEL1	0x0100			/* bit.8  if bit8,9 = 01 then OVMOD1							*/
#define OVMSEL2	0x0200			/* bit.9  if bit8,9 = 10 or 11 then OVMOD2  					*/
#define OVMMOD		0x0400			/* bit.10 modulation modification enable						*/
#define V_FB		0x0800			/* bit.11 �d��FB������ߊE���I��								*/
#define DIDTSEL	0x1000			/* bit.12 L di/dt cmp select(1:di/dt,0:def)						*/
#define V_FB2		0x2000			/* bit.13 �d��FB������ߊE���I��2								*/
#define INT_ST		0x4000			/* bit.14 �O�a���ϕ���~										*/
#define F3DSABL	0x8000			/* bit.15 Notch filter3 Disable									*/


/****************************************************************************************************/
/*		CSW2 (Control Select Switch2 )																*/
/****************************************************************************************************/
#define F4DSABL	0x0001			/* bit.0  Notch filter4 Disable									*/
#define F5DSABL	0x0002			/* bit.1  Notch filter5 Disable									*/
#define F6DSABL	0x0004			/* bit.2  Notch filter6 Disable									*/

/****************************************************************************************************/
/*		�r�b�g�p�����[�^ ( ���������Z�b�g CPU��ASIC )												*/
/****************************************************************************************************/
typedef struct _MICRO_BITIPRM
{
	USHORT	data;
} MICRO_BITIPRM;
#define UPGDIVOUT	0x0001			/* bit.0  �}�C�N�������@�\�g�p�I��								*/
//;--				0x0002			/* bit.1  --Reserve--											*/
//;--				0x0004			/* bit.2  --Reserve--											*/
//;--				0x0008			/* bit.3  --Reserve--											*/
//;--				0x0010			/* bit.4  --Reserve--											*/
//;--				0x0020			/* bit.5  --Reserve--											*/
//;--				0x0040			/* bit.6  --Reserve--											*/
//;--				0x0080			/* bit.7  --Reserve--											*/
//;--				0x0100			/* bit.8  --Reserve--											*/
//;--				0x0200			/* bit.9  --Reserve--											*/
//;--				0x0400			/* bit.10 --Reserve--											*/
//;--				0x0800			/* bit.11 --Reserve--											*/
//;--				0x1000			/* bit.12 --Reserve--											*/
//;--				0x2000			/* bit.13 --Reserve--											*/
//;--				0x4000			/* bit.14 --Reserve--											*/
//;--				0x8000			/* bit.15 --Reserve--											*/


/****************************************************************************************************/
/*		�r�b�g�f�[�^ ( ���������Z�b�g CPU��ASIC�A�X�L�������� ASIC��CPU )							*/
/****************************************************************************************************/
typedef struct _MICRO_BITDAT
{
	USHORT	data;
} MICRO_BITDAT;
#define SERXERR		0x0001			/* bit.0  Senc Receive Error									*/
#define ACCCHKENA	0x0002			/* bit.1  Encoder Acc. Check Enable Flag						*/
//;--				0x0004			/* bit.2  --Reserve--											*/
//;--				0x0008			/* bit.3  --Reserve--											*/
//;--				0x0010			/* bit.4  --Reserve--											*/
//;--				0x0020			/* bit.5  --Reserve--											*/
//;--				0x0040			/* bit.6  --Reserve--											*/
//;--				0x0080			/* bit.7  --Reserve--											*/
//;--				0x0100			/* bit.8  --Reserve--											*/
//;--				0x0200			/* bit.9  --Reserve--											*/
//;--				0x0400			/* bit.10 --Reserve--											*/
//;--				0x0800			/* bit.11 --Reserve--											*/
//;--				0x1000			/* bit.12 --Reserve--											*/
//;--				0x2000			/* bit.13 --Reserve--											*/
//;--				0x4000			/* bit.14 --Reserve--											*/
//;--				0x8000			/* bit.15 --Reserve--											*/



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
/*		ASIC JL-086 SERVOIP ���W�X�^�A�N�Z�X�p�\���̒�`											*/
/*																									*/
/****************************************************************************************************/
typedef struct {
	ASIC_HW_IF_STR		*AsicHwReg;			/* ASIC H/W���W�X�^										*/
	ASIC_U_IF_STR		*AsicMicroReg;		/* ASIC Micro���W�X�^									*/
	MICRO_BITIPRM		MicroBitIprm;		/* MicroBitIprm���W�X�^									*/
	MICRO_BITDAT		MicroBitDat;		/* MicroBitDat���W�X�^									*/
	MICRO_CSW			MicroCsw;			/* MicroCsw���W�X�^										*/
	MICRO_CSW			MicroCsw2;			/* MicroCsw���W�X�^										*//*<<S16F>>*/
	BOOL				MicroRunningSts;	/* Micro�X�e�[�^�X�m�F�p								*/
#if defined(_VC_DEBUG_) || defined(_ROM_SIM_DEBUG_)
	USHORT				*pDummyMicroSram;	/* Dummy buffer for Micro Program download				*/
#endif /* _VC_DEBUG_ */
} ASICS;







/***************************************** end of file **********************************************/
#endif
