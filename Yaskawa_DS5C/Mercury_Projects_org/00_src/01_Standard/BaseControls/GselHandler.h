#ifndef	GSEL_HANDLER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		GselHandler.h : �Q�C���؂�ւ��p�n���h����`												*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�Q�C���ؑ֗p�p�����[�^�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct GSELGAINS {
	KSGAIN		Kp;									/* �ʒu���[�v�Q�C��								*/
	KSGAIN		Kv;									/* ���x���[�v�Q�C��								*/
	KSGAIN		Kv2;								/* ���x���[�v�Q�C���Q							*/
	KSGAIN		Kv2Inv;								/* ���x���[�v�Q�C���Q�t��						*/
	KSGAIN		Kvi;								/* ���x���[�v�ϕ��Q�C��							*/
	KSGAIN		Klpf;								/* �P���x��g���N�t�B���^�Q�C��					*/
	KSGAIN		KpInv;								/* �ʒu���[�v�Q�C���t�� SvCycle�P��(SGDV���g�p)	*/
	KSGAIN		VirtualPosKp;						/* ���z�ʒu�΍��v�Z�p�Q�C��						*/
	KSGAIN		ZcKp;								/* �[���N�����v�p�ʒu���[�v�Q�C��				*/
} GSELGAINS;


#if 0 /* 2010.11.10 Y.Oka ���g�p�̂��߃R�����g�A�E�g */
typedef	struct MICROGAINS {
	long		Kp;									/* �ʒu���[�v�Q�C��								*/
	long		Kv;									/* ���x���[�v�Q�C��								*/
	long		Kvi;								/* ���x���[�v�ϕ��Q�C��							*/
	long		Klpf;								/* �P���x��g���N�t�B���^�Q�C��					*/
} MICROGAINS;
#endif /* #if 0 */


#if 0		/* <S09B> */
typedef	struct GSELDOBS {
	KSGAIN		Ks;									/* �O���I�u�U�[�oKs�Q�C��						*/
	KSGAIN		Ksj;								/* �O���I�u�U�[�oKsj�Q�C��						*/
} GSELDOBS;
#else 		/* <S09B> */
typedef	struct GSELDOBS {					/* �O���I�u�U�[�o�p�p�����[�^��`						*/
	KSGAIN				Ks;						/* �O���I�u�U�[�o�Q�C��								*/
	KSGAIN				Kj;						/* �O���I�u�U�[�o�C�i�[�V���Q�C��					*/
	KSGAIN				Ksj;					/* �O���I�u�U�[�o�g���N�Q�C��						*/
	KSGAIN				Kd;						/* �O���g���N�W��									*/
	KSGAIN				Klpf;					/* �O���I�u�U�[�o���[�p�X�t�B���^�Q�C��				*/
} GSELDOBS;
#endif

#if 0	// �s�v�̂��ߍ폜
typedef struct GSELSIG {						/* �����Q�C���ؑ֏����M��							*/
	BOOL		CoinOn;								/* COIN On�Őؑ�								*/
	BOOL		CoinOff;							/* COIN Off�Őؑ�								*/
	BOOL		NearOn;								/* NEAR On�Őؑ�								*/
	BOOL		NearOff;							/* NEAR Off�Őؑ�								*/
	BOOL		RefZOn;								/* REFZ On�Őؑ�								*/
	BOOL		RefZOff;							/* REFZ Off�Őؑ�								*/
} GSELSIG;
#endif

#if 0	/* <S09B> */
typedef	struct ATGSEL {							/* �����Q�C���ؑ֗p�p�����[�^���ϐ���`				*/
		struct	{								/* �����Q�C���ؑ֗p�p�����[�^						*/
		LONG		Wait;							/* �Q�C���֑ؑ҂�����							*/
/* 2010.11.10 Y.Oka �����Q�C���؂�ւ����̃X���[�W���O���Ή��̂��߃R�����g�A�E�g�@*/
//		LONG		AmondGain;						/* �A�i���O���j�^�o�͗p ���Q�C��				*/
//		GSELGAINS	dGain;							/* ���Q�C��										*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* �����Q�C���ؑ֗p�ϐ�								*/
		LONG		Timer;							/* �Q�C���֑ؑ҂����ԗp�^�C�}					*/
/* 2010.11.10 Y.Oka �����Q�C���؂�ւ����̃X���[�W���O���Ή��̂��߃R�����g�A�E�g�@*/
//		GSELGAINS	*prevGain;						/* �O��Q�C��									*/
//		GSELGAINS	*nextGain;						/* ����Q�C��									*/
	} var;
} ATGSEL;
#endif


#if 0 /* 2010.11.10 Y.Oka �����Q�C���؂�ւ����̃X���[�W���O���Ή��̂��߃R�����g�A�E�g�@*/
typedef	struct ATGSELDOBS {						/* �����Q�C���ؑ֗p�p�����[�^&�ϐ���`(���C�⏞��p)*/
		struct	{								/* �����Q�C���ؑ֗p�p�����[�^						*/
		GSELDOBS	dGain;							/* ���Q�C��										*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* �����Q�C���ؑ֗p�ϐ�								*/
		GSELDOBS	*prevGain;						/* �O��Q�C��									*/
		GSELDOBS	*nextGain;						/* ����Q�C��									*/
	} var;
} ATGSELDOBS;
#endif /* #if 0 */

/*--------------------------------------------------------------------------------------------------*/
/*		<S09B> �����Q�C���ؑ֗p�p�����[�^�ϐ���`													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ATGSEL {							/* �����Q�C���ؑ֗p�p�����[�^���ϐ���`				*/
		struct	{								/* �����Q�C���ؑ֗p�p�����[�^						*/
		LONG		Wait;							/* �Q�C���֑ؑ҂�����							*/
		LONG		AmondGain;						/* �A�i���O���j�^�o�͗p ���Q�C��				*/
		LONG		SwTime;							/* �Q�C���ؑ֎���[ms]							*/
		LONG		SvCycleUs;						/* �������[us]									*/
		LONG		GselTrqFilMin;					/* @@											*/
		GSELGAINS	dGain;							/* ���Q�C��										*/
		GSELDOBS	dGainDobs;						/* ���Q�C��(�O���I�u�U�[�o)						*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* �����Q�C���ؑ֗p�ϐ�								*/
		LONG		Timer;							/* �Q�C���֑ؑ҂����ԗp�^�C�}					*/
		GSELGAINS	*prevGain;						/* �O��Q�C��									*/
		GSELGAINS	*nextGain;						/* ����Q�C��									*/
		GSELDOBS	*prevGainDobs;					/* �O��Q�C��(�O���I�u�U�[�o)					*/
		GSELDOBS	*nextGainDobs;					/* ����Q�C��(�O���I�u�U�[�o)					*/
	} var;
} ATGSEL;

typedef	struct GAIN_CHNG_HNDL {
//	GSELSIG			GselSig;			/* �����Q�C���ؑ֏����M��									*/
	ATGSEL			AtGselA;			/* �����Q�C���ؑ֗p�ϐ�(TrgA)								*/
	ATGSEL			AtGselB;			/* �����Q�C���ؑ֗p�ϐ�(TrgB)								*/
	GSELGAINS		GselGains[6];		/* �T�[�{�Q�C���ؑ֗p�p�����[�^�ϐ�							*/
	GSELDOBS		GselDobs[3];		/* �Q�C���؂�ւ��p�p�����[�^�ϐ�(���C�⏞��p)				*/
	BOOL			AtGsel;				/* �����Q�C���֑ؑI��ݒ�									*/
	LONG			AtGselTrgA;			/* �����Q�C���ؑ֏����`�I��ݒ�								*/
	LONG			AmonActGain;		/* AnaMon : �L���Q�C��										*/
	LONG			ActGain;			/* Un014  : �L���Q�C��										*/
} GAIN_CHNG_HNDL;



#define	GSEL_HANDLER_H
#endif /* GSEL_HANDLER_H */
/***************************************** end of file **********************************************/
