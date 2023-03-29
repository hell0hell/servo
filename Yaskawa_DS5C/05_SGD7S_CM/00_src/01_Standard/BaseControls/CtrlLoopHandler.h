#ifndef CTRL_LOOP_HANDLER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CtrlLoopHandler.h : ���䃋�[�v�n���h����`													*/
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
#include "Basedef.h"
#include "BacklashComp.h"
#include "MLib.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define	MAX_NETCYCLE			4000/125		/* NET�v���ő�҂�����(�`�������^�������)			*/



/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�[������~���[���N�����v����p�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ZX_CTRL_HNDL {					/* �[��������p�萔���ϐ���`						*/
	struct	{									/* �[��������p�萔�f�[�^��`						*/
		LONG	Kp;									/* �ʒu������Q�C��							*/
		LONG	SpdLimit;							/* ���x���䃊�~�b�g�l							*/
		LONG	OrgSrchSpeed;						/* ���_�T�[�`���x				[2^24/OvrSpd]	*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* �[��������p�ϐ��f�[�^��`						*/
		LONG	PosError;							/* �ʒu�΍�										*/
		BOOL	OrgSrchFinish;						/* ���_�T�[�`����flg							*/
	} var;
} ZX_CTRL_HNDL;


/*--------------------------------------------------------------------------------------------------*/
/*		Mode Switch Select definition																*/
/*--------------------------------------------------------------------------------------------------*/
enum MODE_SW_TYPE {
	MODESW_TRQREF = 0x00,						/* ���[�h�X�C�b�` �g���N�w��						*/
	MODESW_SPDREF = 0x01,						/* ���[�h�X�C�b�` ���x�w��							*/
	MODESW_MOTACC = 0x02,						/* ���[�h�X�C�b�` �����x							*/
	MODESW_POSERR = 0x03,						/* ���[�h�X�C�b�` �ʒu�΍�							*/
	MODESW_NOTUSE = 0x04,						/* ���[�h�X�C�b�` �s�g�p							*/
};
/*--------------------------------------------------------------------------------------------------*/
/*		���[�hSW�p�p�����[�^�ϐ���`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MODE_SW {						/* ���[�hSW�p�p�����[�^���ϐ���`					*/
	struct {									/* ���[�hSW�p�p�����[�^��`							*/
		LONG		ModeSWSel;						/* ���[�h�X�C�b�`�I��							*/
		LONG		MSWTrqLevel;					/* ���[�h�X�C�b�`�g���N���x��					*/
		LONG		MSWSpdLevel;					/* ���[�h�X�C�b�`���x���x��						*/
		LONG		MSWAccLevel;					/* ���[�h�X�C�b�`�����x���x��					*/
		LONG		MSWErrLevel;					/* ���[�h�X�C�b�`�΍����x��						*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* ���[�hSW�p�ϐ��f�[�^��`							*/
		BOOL		ModeSWFlag;						/* ���[�h�X�C�b�`ON Flag						*/
		LONG		Acceleration;					/* ���[�h�X�C�b�`�p�����x						*/
	} var;
} MODE_SW;


/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��Z�o�p�p�����[�^��`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct PERRB {								/* �ʒu�΍��쐬�p�\���̒�`						*/
	LONG		LastdPosRef;						/* �O��ʒu�w�ߑ����l[�w�ߒP��]					*/
	LONG		PerrComp;							/* �ʒu���Z�p�␳�l[�w�ߒP��]					*/
	LONG		per32s;								/* �ʒu�΍��o��(�l�̌ܓ�)						*/
	LONG		per32a;								/* �����Βl(�l�̌ܓ�)							*/
} PERRB;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���䉉�Z�p�ϐ���`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POS_LOOP_CTRL {						/* �ʒu����p�萔���ϐ���`							*/
//	struct	{									/* �ʒu����p�萔�f�[�^��`							*/
//		LONG	Kpi;								/* �ʒu������ϕ��Q�C��						*/
//		LONG	Kpd;								/* �ʒu����������Q�C��						*/
//		LONG	VelBias;							/* �ʒu���䑬�x�o�C�A�X							*/
//		LONG	BiasAddLevel;						/* ���x�o�C�A�X���Z��		[Pulse, mm]			*/
//		LONG	KdPff;								/* �ʒu����ʒu�����l�e�e�Q�C��					*/
//		LONG	KdPffFil;							/* �ʒu����ʒu�����l�e�e�t�B���^�Q�C��			*/
//	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* �ʒu����p�ϐ��f�[�^��`							*/
		BOOL	LoopStart;
//		LONG	SpdRef;								/* �ʒu����o��									*/
		LONG	PerIvar64[2];						/* �ʒu�΍��ϕ����Z�p							*/
//		LONG	PacOut;								/* ��ᐧ��o��									*/
//		LONG	IacOut;								/* �ϕ�����o��									*/
//		LONG	DacOut;								/* ��������o��									*/
//		LONG	LastPacOut;							/* �O���ᐧ��o��								*/
//		LONG	dPFfFilo;							/* �ʒu�����l�e�e�t�B���^�o��					*/
	} var;
} POS_LOOP_CTRL;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�Ǘ��p�p�����[�^��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POS_MNG_HNDL {					/* �ʒu�Ǘ��p�f�[�^��`								*/
	struct	{									/* �ʒu�Ǘ��p�萔�f�[�^��`							*/
		EGEAR		InvalidEgear;					/* �d�q�M�A�@�\�������M�A��(1:1)				*/
		EGEAR		Egear;							/* �d�q�M�A����								*/
		UCHAR		CoinOutSel;						/* /COIN�o�̓^�C�~���O�I��ݒ�					*/
		LONG		CoinLevel;						/* �ʒu���ߊ�����					[�w�ߒP��]	*/
		LONG		NearLevel;						/* Near�M����						[�w�ߒP��]	*/
		LONG		PosErrDelayShift;				/* �ʒu�΍����j�^�p�␳���ԃV�t�g��				*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* �ʒu�Ǘ��p�萔�f�[�^��`							*/
		PERRA		PosErrA;						/* �ʒu�΍�(�w�ߒP��)					<S14C>	*/
		LONG		dPcmdEgear;						/* �ʒu�w�ߓd�q�M���o��							*/
		LONG		dPcmdEgrem;						/* �ʒu�w�ߓd�q�M�����Z�]��						*/
		LONG		dPcmda;							/* �ʒu�w�ߑ����l����[�w�ߒP��]					*/
		LONG		PosErr;							/* �ʒu�A���v�΍�(Pulse)				<S14C>	*/
		LONG		Per64[2];						/* �ʒu�A���v�΍����Z�p(Pulse)					*/
//		LONG		LastdPcmda;						/* �O��ʒu�w�ߑ����l[�w�ߒP��]					*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL		EgearDisable;					/* �d�q�M������									*/
		BOOL		PcmdFilStop;					/* �ʒu�w�߃t�B���^����							*/
//		UCHAR		MCphOutTime;					/* ���[�^C���o�͎��Ԑ���p						*/
//		UCHAR		FCphOutTime;					/* �t���NC���o�͎��Ԑ���p						*/
//		UCHAR		SpareUc[2];						/* �\��(Alignment�����p)						*/
	/*----------------------------------------------------------------------------------------------*/
	} var;
	EGEAR		*Egear;								/* �d�q�M�A�؂�ւ��p�|�C���^					*/
	BLSHCMP		BlshCmp;							/* �o�b�N���b�V�␳���Z�p						*/
	APOSRG		PgPos;								/* �@�B���W�n��FB�ʒu							*/
	APOSRG		MencPos;							/* ���[�^PG��FB�ʒu								*/
	APOSRG		FencPos;							/* �t���NPG��FB�ʒu								*/
	LONG		VirtualPositionError;				/* ���z�ʒu�΍�(�w�ߒP��)				<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2012.09.07 Y.Oka ���b�� ���̍\���́A�������͏������̌������� */
	LONG			dPosRefi;				/* �ʒu����F�ʒu�w�ߍ�������			[Pulse/Scan]	*//* <S187> */
	LONG			CompdPosRefi;			/* �ʒu�w�ߍ�������(ScanB->ScanA)		[Pulse/Scan]	*/
	LONG			CompdPcmda;				/* �ʒu�w�ߑ����l����(ScanB->ScanA)		[�w�ߒP��]		*/
	USHORT			RefRenewCntA;			/* �ʒu�w�ߍX�V�����pBaseLoop�J�E���^					*/
	USHORT			RefRenewCntB;			/* �ʒu�w�ߍX�V�����pBaseCtrl�J�E���^					*/
	USHORT			RefRenewalChkCnt;		/* �ʒu�w�ߍX�V�`�F�b�N�p								*/
/* 2012.09.07 Y.Oka ���b�� ���̍\���́A�������͏������̌������� */
} POS_MNG_HNDL;


/*--------------------------------------------------------------------------------------------------*/
/*		���䉉�Z�p�p�����[�^��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CTRL_CMD_PRM {
	struct	{								/* �������X�p��`										*/
		KSGAIN			Klpf3;					/* 1���x��t�B���^�Q�C��(�g���N�⏞��)				*/
		KSGAIN			SpdRefPhFil;			/* �ʑ��i�ݗpLPF									*/
		KSGAIN			SpdRefPhFilGn;			/* �ʑ��i�ݕ⏞�Q�C��								*/
	} TLPrm;
	KSGAIN			OverTrqLoopGain[3];		/* ���x�����p���[�v�Q�C��								*/
	KSGAIN			OverTrqIntegGain[3];	/* ���x�����p�ϕ��Q�C��									*/
	LONG			OverTrqIntegBuf[2];		/* Over Torque Integral Gain Calculation Buffer			*/
	KSGAIN			Klpf;					/* 1���x��t�B���^�Q�C��								*/
	KSGAIN			Klpf2[4];				/* 2���x��t�B���^�Q�C��								*/
	KSGAIN			PI_rate;				/* ���x����PI���䊄��									*/
	KSGAIN			KVfbFil;				/* ���xFB�t�B���^�Q�C��									*/
	KSGAIN			Kpi;					/* �ʒu������ϕ��Q�C��								*/
	LONG			SvonSpdLmtValue;		/* �T�[�{�I�������x�����l								*/
	/*----------------------------------------------------------------------------------------------*/
	KSGAIN			KpiFc;					/* �t���N�p�ʒu������ϕ��Q�C��						*/
	KSGAIN			KdPff;					/* �ʒu����ʒu�����lFF�Q�C��							*/
	KSGAIN			KdPffFil;				/* �ʒu����ʒu�����lFF�t�B���^�Q�C��					*/
	BOOL			LpassFil2;				/* 2���x��t�B���^�I��									*/
	BOOL			LpassFil3;				/* �g���N�⏞��1���x��t�B���^�I��						*/
	/*----------------------------------------------------------------------------------------------*/
	KSGAIN			KVrfFil;				/* ���x�w�߃t�B���^�Q�C��								*/
	BOOL			IPSpdControl;			/* I-P����I��ݒ�										*/
	BOOL			SpdIctrlKeep;			/* ���x����ϕ��ێ�										*/
	BOOL			VrefSpdLmt;				/* ���x�w�߂��O�����x�����Ƃ��Ďg�p						*/
#if (FLOAT_USE==TRUE)
	float			TConSpdLmtLevel;		/* �g���N���䎞�̑��x�������x��		 [2^24/OvrSpd]		*/
	float			SpdSfsAcc;				/* �\�t�g�X�^�[�g������									*/
	float			SpdSfsDec;				/* �\�t�g�X�^�[�g������									*/
	float			SvonSpdLmtLevel;		/* �T�[�{�I�������x�������x��							*/
	float			SpdLdstpDec;			/* ����������~������(Pn30A)							*/ /* <S1F5> */
#else
	LONG			TConSpdLmtLevel;		/* �g���N���䎞�̑��x�������x��		 [2^24/OvrSpd]		*/
	LONG			SpdSfsAcc;				/* �\�t�g�X�^�[�g������									*/
	LONG			SpdSfsDec;				/* �\�t�g�X�^�[�g������									*/
	LONG			SvonSpdLmtLevel;		/* �T�[�{�I�������x�������x��							*/
	LONG			SpdLdstpDec;			/* ����������~������(Pn30A)							*/ /* <S1F5> */
#endif /* FLOAT_USE */
} CTRL_CMD_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		���䉉�Z�p���[�N��`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CTRL_CMD_MNG {
	LONG			CtrlMode;				/* ��{���[�v���䃂�[�h									*/
	LONG			LastCtrlMode;			/* �O���{���[�v���䃂�[�h								*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float			TrqRefo;				/* �g���N�w�ߏo��					[2^24/MaxTrq]		*/
	float			SpdRefo;				/* ���x�w�ߏo��						[2^24/OvrSpd]		*/
	float			SpdLdstpRefo;			/* ����������~���x�w�ߏo��								*/ /* <S1F5> */
	LONG			dPosRefo;				/* �ʒu�w�ߍ����o��					[Pulse/Scan]		*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			BaseFbkPos;				/* ����؂�ւ����̃��[�^�ʒu		[Pulse]				*/
	LONG			dPosFbki;				/* �ʒu����F�ʒu�e�a��������		[Pulse/Scan]		*/
	LONG			PosFbki;				/* �ʒu����F�ʒu�e�a����			[Pulse/Scan]		*/
	LONG			FbkPosFromCpos;			/* ���_����̃��[�^�ʒu		 [+/-(PulseNo/2)],[LM:float]	*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL			TrqFilClrCmd;			/* �g���N�w�߃t�B���^�ϕ��������v��						*/
	BOOL			TrqFilDisable;			/* �g���N�w�߃t�B���^�����v��							*/
//	float			TrqInputPos;			/* �g���N�O���w�ߓ��͈ʒu�I��							*//* <S0B7> */
	float			SpdLimit;				/* �g���N���䎞�̑��x�����l								*/
	float			OverTrqIntegBuf[2];		/* �g���N���䎞�̑��x�����p�ϕ��o�b�t�@					*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			OverTrq;				/* @@ Over Torque [2^24/MaxTrq]							*/
	BOOL			SpdClamp;				/* @@ Speed Clamp Flag on Torque Control Mode			*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL			TrqClamp;				/* <S045> Torque Clamp Flag								*/
/*--------------------------------------------------------------------------------------------------*/
	float			SpdRefi;				/* ���x�w�ߓ���						[2^24/OvrSpd]		*/
	float			SpdRfFilo;				/* ���x�w�߃t�B���^�o��				[2^24/OvrSpd]		*/
	float			SpdFbki;				/* ���xFB����						[2^24/OvrSpd]		*/
	float			SpdObsFbki;				/* ���xFB����(OBS��)				[2^24/OvrSpd]		*/
	float			VelIvar64[2];			/* �ϕ����䉉�Z�p(����؂�ւ����V���b�N�ጸ�p)			*/
/*--------------------------------------------------------------------------------------------------*/
	float			PosRefLo;				/* �w�߈ʒu(Low)					[�w�ߒP��]			*/
	float			PosRef;					/* �ʒu���䎞�̎w�߈ʒu				[�w�ߒP��]			*/
	PMAFV			Pmafvar;				/* �ʒu�w�߈ړ����σt�B���^���Z�p�ϐ�					*/
	LONG			dPcmdFilo;				/* �ʒu�w�߃t�B���^�o��				[Pulse/Scan]		*/
	LONG			Pmafbuf[MAX_NETCYCLE];	/* �ʒu�w�߈ړ����σt�B���^���Z�pBUF					*/
	float			dPFfFilo;				/* �ʒu�����lFF�t�B���^�o��								*/
	BOOL			PicvClrCmd;				/* �ʒu�΍��ϕ��N���A�v��								*/
	BOOL			PerrClrReq;				/* �ʒu�΍��N���A�v��(�e�[�u���^�]/PJOG��p)			*/
	BOOL			PerClrEdge;				/* @@  �ʒu�΍��N���A�G�b�W�v��(ANLG�p)					*/
	LONG			ClrFbPos;				/* @@  �ʒu�΍��N���A���b�`�ʒu(ANLG�p)					*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			SpdFFCx;				/* �g������F���xFF�⏞				[2^24/OvrSpd]		*/
	float			SpdFBCx;				/* �g������F���xFB�⏞				[2^24/OvrSpd]		*//* 2013/04/01��MD�ł́A���ı�Ă���Ă���	*/	
	LONG			TrqFFCx;				/* �g������F�g���NFF�⏞			[2^24/MaxTrq]		*/
	float			TrqFBCx;				/* �g������F�g���NFB�⏞			[2^24/MaxTrq]		*/
#else
	LONG			TrqRefo;				/* �g���N�w�ߏo��					[2^24/MaxTrq]		*/
	LONG			SpdRefo;				/* ���x�w�ߏo��						[2^24/OvrSpd]		*/
	LONG			SpdLdstpRefo;			/* ����������~���x�w�ߏo��								*/ /* <S1F5> */
	LONG			dPosRefo;				/* �ʒu�w�ߍ����o��					[Pulse/Scan]		*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			BaseFbkPos;				/* ����؂�ւ����̃��[�^�ʒu		[Pulse]				*/
	LONG			dPosFbki;				/* �ʒu����F�ʒu�e�a��������		[Pulse/Scan]		*/
	LONG			PosFbki;				/* �ʒu����F�ʒu�e�a����			[Pulse/Scan]		*/
	LONG			FbkPosFromCpos;			/* ���_����̃��[�^�ʒu		 [+/-(PulseNo/2)],[LM:LONG]	*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL			TrqFilClrCmd;			/* �g���N�w�߃t�B���^�ϕ��������v��						*/
	BOOL			TrqFilDisable;			/* �g���N�w�߃t�B���^�����v��							*/
//	LONG			TrqInputPos;			/* �g���N�O���w�ߓ��͈ʒu�I��							*//* <S0B7> */
	LONG			SpdLimit;				/* �g���N���䎞�̑��x�����l								*/
	LONG			OverTrqIntegBuf[2];		/* �g���N���䎞�̑��x�����p�ϕ��o�b�t�@					*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			OverTrq;				/* @@ Over Torque [2^24/MaxTrq]							*/
	BOOL			SpdClamp;				/* @@ Speed Clamp Flag on Torque Control Mode			*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL			TrqClamp;				/* <S045> Torque Clamp Flag 							*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			SpdRefi;				/* ���x�w�ߓ���						[2^24/OvrSpd]		*/
	LONG			SpdRfFilo;				/* ���x�w�߃t�B���^�o��				[2^24/OvrSpd]		*/
	LONG			SpdFbki;				/* ���xFB����						[2^24/OvrSpd]		*/
	LONG			SpdObsFbki;				/* ���xFB����(OBS��)				[2^24/OvrSpd]		*/
	LONG			VelIvar64[2];			/* �ϕ����䉉�Z�p(����؂�ւ����V���b�N�ጸ�p)			*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			PosRefLo;				/* �w�߈ʒu(Low)					[�w�ߒP��]			*/
	LONG			PosRef;					/* �ʒu���䎞�̎w�߈ʒu				[�w�ߒP��]			*/
	PMAFV			Pmafvar;				/* �ʒu�w�߈ړ����σt�B���^���Z�p�ϐ�					*/
	LONG			dPcmdFilo;				/* �ʒu�w�߃t�B���^�o��				[Pulse/Scan]		*/
	LONG			Pmafbuf[MAX_NETCYCLE];	/* �ʒu�w�߈ړ����σt�B���^���Z�pBUF					*/
	LONG			dPFfFilo;				/* �ʒu�����lFF�t�B���^�o��								*/
	BOOL			PicvClrCmd;				/* �ʒu�΍��ϕ��N���A�v��								*/
	BOOL			PerrClrReq;				/* �ʒu�΍��N���A�v��(�e�[�u���^�]/PJOG��p)			*/
	BOOL			PerClrEdge;				/* @@  �ʒu�΍��N���A�G�b�W�v��(ANLG�p)					*/
	LONG			ClrFbPos;				/* @@  �ʒu�΍��N���A���b�`�ʒu(ANLG�p)					*/
/*--------------------------------------------------------------------------------------------------*/
	LONG			SpdFFCx;				/* �g������F���xFF�⏞				[2^24/OvrSpd]		*/
	LONG			SpdFBCx;				/* �g������F���xFB�⏞				[2^24/OvrSpd]		*//* 2013/04/01��MD�ł́A���ı�Ă���Ă���	*/	
	LONG			TrqFFCx;				/* �g������F�g���NFF�⏞			[2^24/MaxTrq]		*/
	LONG			TrqFBCx;				/* �g������F�g���NFB�⏞			[2^24/MaxTrq]		*/
#endif /* FLOAT_USE */
} CTRL_CMD_MNG;



#define CTRL_LOOP_HANDLER_H
#endif /* CTRL_LOOP_HANDLER_H */
/***************************************** end of file **********************************************/
