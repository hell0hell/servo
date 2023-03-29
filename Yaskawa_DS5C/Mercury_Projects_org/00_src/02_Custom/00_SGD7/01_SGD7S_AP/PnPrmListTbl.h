/****************************************************************************
  Description : Register and Parameter Access Interface
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2013
                  All Rights Reserved

  Project       : Mercury

  Create Date   : 2014/04/09

****************************************************************************/
#ifndef __PN_PRM_DEF_TBL_H 
#define __PN_PRM_DEF_TBL_H

#include "PrmAccessIf.h"


extern const PRMTBL PnPrmTbl[];
extern const ULONG PnPrmTblEntNum;

extern const PRM_ATTR_T pndef_b_prm0;
extern const PRM_ATTR_T pndef_b_prm1;
extern const PRM_ATTR_T pndef_b_prm2;
extern const PRM_ATTR_T pndef_b_prm6;
extern const PRM_ATTR_T pndef_b_prm7;
extern const PRM_ATTR_T pndef_b_prm8;
extern const PRM_ATTR_T pndef_b_prm9;
extern const PRM_ATTR_T pndef_b_prmA;
extern const PRM_ATTR_T pndef_b_prmB;
extern const PRM_ATTR_T pndef_b_prmC;
extern const PRM_ATTR_T pndef_b_prmD;
extern const PRM_ATTR_T pndef_b_prmE;
extern const PRM_ATTR_T pndef_b_prmF;
extern const PRM_ATTR_T pndef_AxisAddress0;
extern const PRM_ATTR_T pndef_AxisAddress1;
extern const PRM_ATTR_T pndef_UniPrmSel;
extern const PRM_ATTR_T pndef_CycleSwitch;
extern const PRM_ATTR_T pndef_b_prm80;
extern const PRM_ATTR_T pndef_b_prm81;
extern const PRM_ATTR_T pndef_loophz;
extern const PRM_ATTR_T pndef_pitime;
extern const PRM_ATTR_T pndef_posgn;
extern const PRM_ATTR_T pndef_jrate;
extern const PRM_ATTR_T pndef_loophz2;
extern const PRM_ATTR_T pndef_pitime2;
extern const PRM_ATTR_T pndef_posgn2;
extern const PRM_ATTR_T pndef_ffgn;
extern const PRM_ATTR_T pndef_fffil;
extern const PRM_ATTR_T pndef_gnmode;
extern const PRM_ATTR_T pndef_mdswlv;
extern const PRM_ATTR_T pndef_mswrf;
extern const PRM_ATTR_T pndef_mswacc;
extern const PRM_ATTR_T pndef_msplvl;
extern const PRM_ATTR_T pndef_pitfil;
extern const PRM_ATTR_T pndef_dobgn;
extern const PRM_ATTR_T pndef_dobgn2;
extern const PRM_ATTR_T pndef_dtrqgn;
extern const PRM_ATTR_T pndef_dlpfil;
extern const PRM_ATTR_T pndef_dobjgn;
extern const PRM_ATTR_T pndef_evobgn;
extern const PRM_ATTR_T pndef_evobjgn;
extern const PRM_ATTR_T pndef_gnswfil1;
extern const PRM_ATTR_T pndef_gnswfil2;
extern const PRM_ATTR_T pndef_gnswwait1;
extern const PRM_ATTR_T pndef_gnswwait2;
extern const PRM_ATTR_T pndef_gnsw1;
extern const PRM_ATTR_T pndef_Pn13B;
extern const PRM_ATTR_T pndef_KcGainRatio;
extern const PRM_ATTR_T pndef_mdlsw;
extern const PRM_ATTR_T pndef_mdlgn;
extern const PRM_ATTR_T pndef_mdlzeta;
extern const PRM_ATTR_T pndef_mdlptff;
extern const PRM_ATTR_T pndef_mdlntff;
extern const PRM_ATTR_T pndef_mdlantfrq;
extern const PRM_ATTR_T pndef_mdlresfrq;
extern const PRM_ATTR_T pndef_mdlvff;
extern const PRM_ATTR_T pndef_mdlgn2;
extern const PRM_ATTR_T pndef_mdlzeta2;
extern const PRM_ATTR_T pndef_ff_frq;
extern const PRM_ATTR_T pndef_ff_fil;
extern const PRM_ATTR_T pndef_mfctype;
extern const PRM_ATTR_T pndef_avibsw;
extern const PRM_ATTR_T pndef_avibfrq;
extern const PRM_ATTR_T pndef_avibgn;
extern const PRM_ATTR_T pndef_avibdamp;
extern const PRM_ATTR_T pndef_avibfil1;
extern const PRM_ATTR_T pndef_avibfil2;
extern const PRM_ATTR_T pndef_avibdamp2;
extern const PRM_ATTR_T pndef_avibfrq2;
extern const PRM_ATTR_T pndef_DatLevel;
extern const PRM_ATTR_T pndef_jrateSw;
extern const PRM_ATTR_T pndef_jrate2;
extern const PRM_ATTR_T pndef_jrateChgTime1;
extern const PRM_ATTR_T pndef_jrateChgTime2;
extern const PRM_ATTR_T pndef_mswrf_l;
extern const PRM_ATTR_T pndef_mswacc_l;
extern const PRM_ATTR_T pndef_divlessSw;
extern const PRM_ATTR_T pndef_velffgain;
extern const PRM_ATTR_T pndef_trqffgain;
extern const PRM_ATTR_T pndef_trqfftime;
extern const PRM_ATTR_T pndef_prstsw;
extern const PRM_ATTR_T pndef_mtlmt;
extern const PRM_ATTR_T pndef_cmfilsw;
extern const PRM_ATTR_T pndef_expbias1;
extern const PRM_ATTR_T pndef_fcplsml;
extern const PRM_ATTR_T pndef_ratb2l;
extern const PRM_ATTR_T pndef_rata2l;
extern const PRM_ATTR_T pndef_pgrat2l;
extern const PRM_ATTR_T pndef_expfil;
extern const PRM_ATTR_T pndef_mafil;
extern const PRM_ATTR_T pndef_MulFrq;
extern const PRM_ATTR_T pndef_prefHfilsmooth;
extern const PRM_ATTR_T pndef_prefHfilacctime;
extern const PRM_ATTR_T pndef_fcsel;
extern const PRM_ATTR_T pndef_cmctrlexsw;
extern const PRM_ATTR_T pndef_blcm2l;
extern const PRM_ATTR_T pndef_blfiltime;
extern const PRM_ATTR_T pndef_perrclrsigtime;
extern const PRM_ATTR_T pndef_encbaudrate;
extern const PRM_ATTR_T pndef_scalepgrat;
extern const PRM_ATTR_T pndef_scalepitch2l;
extern const PRM_ATTR_T pndef_psencernum;
extern const PRM_ATTR_T pndef_vrefgn;
extern const PRM_ATTR_T pndef_speed1;
extern const PRM_ATTR_T pndef_speed2;
extern const PRM_ATTR_T pndef_speed3;
extern const PRM_ATTR_T pndef_jogspd;
extern const PRM_ATTR_T pndef_sfsacc;
extern const PRM_ATTR_T pndef_sfsdec;
extern const PRM_ATTR_T pndef_srfil;
extern const PRM_ATTR_T pndef_fbfil;
extern const PRM_ATTR_T pndef_svoffdectime;
extern const PRM_ATTR_T pndef_vrefmafil;
extern const PRM_ATTR_T pndef_vffmafil;
extern const PRM_ATTR_T pndef_vibsel;
extern const PRM_ATTR_T pndef_vibsens;
extern const PRM_ATTR_T pndef_vibnorm;
extern const PRM_ATTR_T pndef_maxvel;
extern const PRM_ATTR_T pndef_Pn324;
extern const PRM_ATTR_T pndef_Pn325;
extern const PRM_ATTR_T pndef_speed1_l;
extern const PRM_ATTR_T pndef_speed2_l;
extern const PRM_ATTR_T pndef_speed3_l;
extern const PRM_ATTR_T pndef_jogspd_l;
extern const PRM_ATTR_T pndef_vibnorm_l;
extern const PRM_ATTR_T pndef_maxvel_l;
extern const PRM_ATTR_T pndef_Pn387;
extern const PRM_ATTR_T pndef_trefgn;
extern const PRM_ATTR_T pndef_trqfil11;
extern const PRM_ATTR_T pndef_tlmtf;
extern const PRM_ATTR_T pndef_tlmtr;
extern const PRM_ATTR_T pndef_outlmf1;
extern const PRM_ATTR_T pndef_outlmr1;
extern const PRM_ATTR_T pndef_emgtrq;
extern const PRM_ATTR_T pndef_tcrlmt;
extern const PRM_ATTR_T pndef_tfuncsw;
extern const PRM_ATTR_T pndef_ntchfil1;
extern const PRM_ATTR_T pndef_ntchq1;
extern const PRM_ATTR_T pndef_ntchdamp1;
extern const PRM_ATTR_T pndef_ntchfil2;
extern const PRM_ATTR_T pndef_ntchq2;
extern const PRM_ATTR_T pndef_ntchdamp2;
extern const PRM_ATTR_T pndef_secfil;
extern const PRM_ATTR_T pndef_secq;
extern const PRM_ATTR_T pndef_trqfil12;
extern const PRM_ATTR_T pndef_treffil;
extern const PRM_ATTR_T pndef_tfuncsw2;
extern const PRM_ATTR_T pndef_ntchfil3;
extern const PRM_ATTR_T pndef_ntchq3;
extern const PRM_ATTR_T pndef_ntchdamp3;
extern const PRM_ATTR_T pndef_ntchfil4;
extern const PRM_ATTR_T pndef_ntchq4;
extern const PRM_ATTR_T pndef_ntchdamp4;
extern const PRM_ATTR_T pndef_ntchfil5;
extern const PRM_ATTR_T pndef_ntchq5;
extern const PRM_ATTR_T pndef_ntchdamp5;
extern const PRM_ATTR_T pndef_RippleCompSw;
extern const PRM_ATTR_T pndef_UvTrqLmt;
extern const PRM_ATTR_T pndef_UvTrqLmtOffFil;
extern const PRM_ATTR_T pndef_tffmafil;
extern const PRM_ATTR_T pndef_VelRippleCompSpd;
extern const PRM_ATTR_T pndef_swpminfrq;
extern const PRM_ATTR_T pndef_swpmaxfrq;
extern const PRM_ATTR_T pndef_detfrqlmt;
extern const PRM_ATTR_T pndef_ezffttrq;
extern const PRM_ATTR_T pndef_ezfftsw;
extern const PRM_ATTR_T pndef_anotchsw;
extern const PRM_ATTR_T pndef_anotchsw2;
extern const PRM_ATTR_T pndef_gcomptrq;
extern const PRM_ATTR_T pndef_pqfriqcomptrq;
extern const PRM_ATTR_T pndef_nqfriqcomptrq;
extern const PRM_ATTR_T pndef_vfrigcomptrq;
extern const PRM_ATTR_T pndef_spdhys;
extern const PRM_ATTR_T pndef_tcrlmt_l;
extern const PRM_ATTR_T pndef_pdetloophz;
extern const PRM_ATTR_T pndef_pdetpitime;
extern const PRM_ATTR_T pndef_tlmtf_l;
extern const PRM_ATTR_T pndef_tlmtr_l;
extern const PRM_ATTR_T pndef_pdetmaxspd;
extern const PRM_ATTR_T pndef_pdetmaxt;
extern const PRM_ATTR_T pndef_pdetclmpt;
extern const PRM_ATTR_T pndef_pdetwait;
extern const PRM_ATTR_T pndef_pdetrepeat;
extern const PRM_ATTR_T pndef_pdetdistok;
extern const PRM_ATTR_T pndef_pdetjrate;
extern const PRM_ATTR_T pndef_pdetintegless;
extern const PRM_ATTR_T pndef_pdetinteglesst;
extern const PRM_ATTR_T pndef_pdetmaxspd_r;
extern const PRM_ATTR_T pndef_pdetdistok_r;
extern const PRM_ATTR_T pndef_pdetmaxtrq;
extern const PRM_ATTR_T pndef_pdetinctime;
extern const PRM_ATTR_T pndef_pdetconsttime;
extern const PRM_ATTR_T pndef_pdetdegreeok;
extern const PRM_ATTR_T pndef_pdetrevlvspdref;
extern const PRM_ATTR_T pndef_pdetrevlvwait;
extern const PRM_ATTR_T pndef_pdetoslevel;
extern const PRM_ATTR_T pndef_clamp;
extern const PRM_ATTR_T pndef_tgonlv;
extern const PRM_ATTR_T pndef_vcmplv;
extern const PRM_ATTR_T pndef_brktim;
extern const PRM_ATTR_T pndef_brkspd;
extern const PRM_ATTR_T pndef_brkwai;
extern const PRM_ATTR_T pndef_acoftm;
extern const PRM_ATTR_T pndef_insel1;
extern const PRM_ATTR_T pndef_insel2;
extern const PRM_ATTR_T pndef_insel3;
extern const PRM_ATTR_T pndef_insel4;
extern const PRM_ATTR_T pndef_outsel1;
extern const PRM_ATTR_T pndef_outsel2;
extern const PRM_ATTR_T pndef_outsel3;
extern const PRM_ATTR_T pndef_inselA;
extern const PRM_ATTR_T pndef_sorev1;
extern const PRM_ATTR_T pndef_sorev2;
extern const PRM_ATTR_T pndef_outsel4;
extern const PRM_ATTR_T pndef_insel5;
extern const PRM_ATTR_T pndef_insel6;
extern const PRM_ATTR_T pndef_outsel6;
extern const PRM_ATTR_T pndef_outsel7;
extern const PRM_ATTR_T pndef_insel7;
extern const PRM_ATTR_T pndef_fpgoverlv2l;
extern const PRM_ATTR_T pndef_overwrnlv;
extern const PRM_ATTR_T pndef_overlv2l;
extern const PRM_ATTR_T pndef_coinlv2l;
extern const PRM_ATTR_T pndef_nearlv2l;
extern const PRM_ATTR_T pndef_svonoverlv2l;
extern const PRM_ATTR_T pndef_svonoverwrnlv;
extern const PRM_ATTR_T pndef_svonspdlmtlv;
extern const PRM_ATTR_T pndef_fpgcutrate;
extern const PRM_ATTR_T pndef_olwarlvl;
extern const PRM_ATTR_T pndef_mtbastrq_drt;
extern const PRM_ATTR_T pndef_svbastrq_drt;
extern const PRM_ATTR_T pndef_powon_mon;
extern const PRM_ATTR_T pndef_pjogsw;
extern const PRM_ATTR_T pndef_pjogdistl;
extern const PRM_ATTR_T pndef_pjogrotspd;
extern const PRM_ATTR_T pndef_pjogacctm;
extern const PRM_ATTR_T pndef_pjogwaittm;
extern const PRM_ATTR_T pndef_pjognum;
extern const PRM_ATTR_T pndef_outalm_no1;
extern const PRM_ATTR_T pndef_outalm_no2;
extern const PRM_ATTR_T pndef_adatgmax;
extern const PRM_ATTR_T pndef_trcalm_no;
extern const PRM_ATTR_T pndef_monoffset1;
extern const PRM_ATTR_T pndef_monoffset2;
extern const PRM_ATTR_T pndef_mongain1;
extern const PRM_ATTR_T pndef_mongain2;
extern const PRM_ATTR_T pndef_powmon_unitime;
extern const PRM_ATTR_T pndef_remdetw;
extern const PRM_ATTR_T pndef_ovserrdetlvl;
extern const PRM_ATTR_T pndef_clamp_l;
extern const PRM_ATTR_T pndef_tgonlv_l;
extern const PRM_ATTR_T pndef_vcmplv_l;
extern const PRM_ATTR_T pndef_brkspd_l;
extern const PRM_ATTR_T pndef_svonspdlmtlv_l;
extern const PRM_ATTR_T pndef_pjoglinspd;
extern const PRM_ATTR_T pndef_msrcra;
extern const PRM_ATTR_T pndef_pldetsw;
extern const PRM_ATTR_T pndef_pot_selsw;
extern const PRM_ATTR_T pndef_not_selsw;
extern const PRM_ATTR_T pndef_dec_selsw;
extern const PRM_ATTR_T pndef_ext1_selsw;
extern const PRM_ATTR_T pndef_ext2_selsw;
extern const PRM_ATTR_T pndef_ext3_selsw;
extern const PRM_ATTR_T pndef_fstp_selsw;
extern const PRM_ATTR_T pndef_pcl_selsw;
extern const PRM_ATTR_T pndef_ncl_selsw;
extern const PRM_ATTR_T pndef_clinkans_selsw;
extern const PRM_ATTR_T pndef_cmcans_selsw;
extern const PRM_ATTR_T pndef_convready_selsw;
extern const PRM_ATTR_T pndef_convmcon_selsw;
extern const PRM_ATTR_T pndef_coin_selsw;
extern const PRM_ATTR_T pndef_vcmp_selsw;
extern const PRM_ATTR_T pndef_tgon_selsw;
extern const PRM_ATTR_T pndef_srdy_selsw;
extern const PRM_ATTR_T pndef_clt_selsw;
extern const PRM_ATTR_T pndef_vlt_selsw;
extern const PRM_ATTR_T pndef_bk_selsw;
extern const PRM_ATTR_T pndef_warn_selsw;
extern const PRM_ATTR_T pndef_near_selsw;
extern const PRM_ATTR_T pndef_cpulse_selsw;
extern const PRM_ATTR_T pndef_clink_selsw;
extern const PRM_ATTR_T pndef_cmcon_selsw;
extern const PRM_ATTR_T pndef_pm_selsw;
extern const PRM_ATTR_T pndef_dao1_selsw;
extern const PRM_ATTR_T pndef_dao2_selsw;
extern const PRM_ATTR_T pndef_regres;
extern const PRM_ATTR_T pndef_dbres;
extern const PRM_ATTR_T pndef_exreger;
extern const PRM_ATTR_T pndef_exdbr;
extern const PRM_ATTR_T pndef_bbondiff0;
extern const PRM_ATTR_T pndef_saftyfuncSw;
extern const PRM_ATTR_T pndef_safeindecA;
extern const PRM_ATTR_T pndef_safeindecB;
extern const PRM_ATTR_T pndef_safeindecVel;
extern const PRM_ATTR_T pndef_safeStopTime;
extern const PRM_ATTR_T pndef_safePerrlvl;
extern const PRM_ATTR_T pndef_safeVreflvl;
extern const PRM_ATTR_T pndef_almmsk1;
extern const PRM_ATTR_T pndef_almmsk2;
extern const PRM_ATTR_T pndef_opdetsw;
extern const PRM_ATTR_T pndef_syssw1;
extern const PRM_ATTR_T pndef_syssw2;
extern const PRM_ATTR_T pndef_syssw3;
extern const PRM_ATTR_T pndef_comfil;
extern const PRM_ATTR_T pndef_InstEnvTmp_sw1;
extern const PRM_ATTR_T pndef_paswrd;
extern const PRM_ATTR_T pndef_styp;
extern const PRM_ATTR_T pndef_ampw;
extern const PRM_ATTR_T pndef_yopt;
extern const PRM_ATTR_T pndef_irats;
extern const PRM_ATTR_T pndef_imaxs;
extern const PRM_ATTR_T pndef_ibs_md;
extern const PRM_ATTR_T pndef_imdt_mxt;
extern const PRM_ATTR_T pndef_olwrat;
extern const PRM_ATTR_T pndef_reger;
extern const PRM_ATTR_T pndef_dbr;
extern const PRM_ATTR_T pndef_wre;
extern const PRM_ATTR_T pndef_wdb;
extern const PRM_ATTR_T pndef_spgfail;
extern const PRM_ATTR_T pndef_PnE1F;
extern const PRM_ATTR_T pndef_ignd;
extern const PRM_ATTR_T pndef_ignq;
extern const PRM_ATTR_T pndef_kid;
extern const PRM_ATTR_T pndef_kiq;
extern const PRM_ATTR_T pndef_limintd;
extern const PRM_ATTR_T pndef_limintq;
extern const PRM_ATTR_T pndef_ifil;
extern const PRM_ATTR_T pndef_shant;
extern const PRM_ATTR_T pndef_vrevgn;
extern const PRM_ATTR_T pndef_lmdata;
extern const PRM_ATTR_T pndef_pwmf;
extern const PRM_ATTR_T pndef_odt_c;
extern const PRM_ATTR_T pndef_odlv_slp;
extern const PRM_ATTR_T pndef_dq_sw;
extern const PRM_ATTR_T pndef_phscmp;
extern const PRM_ATTR_T pndef_kdd;
extern const PRM_ATTR_T pndef_kdq;
extern const PRM_ATTR_T pndef_dfild;
extern const PRM_ATTR_T pndef_dfilq;
extern const PRM_ATTR_T pndef_dq_sw2;
extern const PRM_ATTR_T pndef_motresq;
extern const PRM_ATTR_T pndef_PnE3C;
extern const PRM_ATTR_T pndef_PnE3D;
extern const PRM_ATTR_T pndef_kvvfil;
extern const PRM_ATTR_T pndef_fltrange;
extern const PRM_ATTR_T pndef_tplsfil;
extern const PRM_ATTR_T pndef_mnrng0;
extern const PRM_ATTR_T pndef_mnrng1;
extern const PRM_ATTR_T pndef_ipg_rwt;
extern const PRM_ATTR_T pndef_PnE45;
extern const PRM_ATTR_T pndef_PnE46;
extern const PRM_ATTR_T pndef_PnE47;
extern const PRM_ATTR_T pndef_mnptr0l;
extern const PRM_ATTR_T pndef_mnptr1l;
extern const PRM_ATTR_T pndef_zadjiu;
extern const PRM_ATTR_T pndef_zadjiv;
extern const PRM_ATTR_T pndef_gadjiu_v;
extern const PRM_ATTR_T pndef_zadjvl;
extern const PRM_ATTR_T pndef_gadjv;
extern const PRM_ATTR_T pndef_zadjtp_n;
extern const PRM_ATTR_T pndef_gadjtp_n;
extern const PRM_ATTR_T pndef_mnaz0_1;
extern const PRM_ATTR_T pndef_mnag0_1;
extern const PRM_ATTR_T pndef_pgadj;
extern const PRM_ATTR_T pndef_pnadj;
extern const PRM_ATTR_T pndef_ratbt_mt;
extern const PRM_ATTR_T pndef_ratmtt_2;
extern const PRM_ATTR_T pndef_ratmt2_os;
extern const PRM_ATTR_T pndef_NonRegOvlvl;
extern const PRM_ATTR_T pndef_pnvoltgn;
extern const PRM_ATTR_T pndef_ovlvl;
extern const PRM_ATTR_T pndef_regonlvl;
extern const PRM_ATTR_T pndef_uvlvl;
extern const PRM_ATTR_T pndef_uvfil;
extern const PRM_ATTR_T pndef_regoffhys;
extern const PRM_ATTR_T pndef_vdetadfil;
extern const PRM_ATTR_T pndef_vdefilsw;
extern const PRM_ATTR_T pndef_UvWrnLvl;
extern const PRM_ATTR_T pndef_VdetADMax;
extern const PRM_ATTR_T pndef_ConvW;
extern const PRM_ATTR_T pndef_FanTemperatureOffset;
extern const PRM_ATTR_T pndef_FanTimeOffset;
extern const PRM_ATTR_T pndef_RushLifeTime;
extern const PRM_ATTR_T pndef_DBLifeTime;
extern const PRM_ATTR_T pndef_RushSwichCount;
extern const PRM_ATTR_T pndef_DBSwichCount;
extern const PRM_ATTR_T pndef_ChgWaitTime;
extern const PRM_ATTR_T pndef_PrdyWaitTime;
extern const PRM_ATTR_T pndef_DischCompVlt;
extern const PRM_ATTR_T pndef_scnvorgoffset;
extern const PRM_ATTR_T pndef_phsofst;
extern const PRM_ATTR_T pndef_olspdlvl;
extern const PRM_ATTR_T pndef_ollspdprm;
extern const PRM_ATTR_T pndef_dbol_warn;
extern const PRM_ATTR_T pndef_dbol_alm;
extern const PRM_ATTR_T pndef_regsynonlvl;
extern const PRM_ATTR_T pndef_regsynontime;
extern const PRM_ATTR_T pndef_ctrlboardalmtmpr;
extern const PRM_ATTR_T pndef_pwrboardalmtmpr;
extern const PRM_ATTR_T pndef_ctrlboardalmclrtmpr;
extern const PRM_ATTR_T pndef_pwrboardalmclrtmpr;
extern const PRM_ATTR_T pndef_ifil2;
extern const PRM_ATTR_T pndef_pwmf2;
extern const PRM_ATTR_T pndef_ilimlvl;
extern const PRM_ATTR_T pndef_pwmf3;
extern const PRM_ATTR_T pndef_fbfil2;
extern const PRM_ATTR_T pndef_kv;
extern const PRM_ATTR_T pndef_tv;
extern const PRM_ATTR_T pndef_idlim;
extern const PRM_ATTR_T pndef_v1max;
extern const PRM_ATTR_T pndef_vmaxid;
extern const PRM_ATTR_T pndef_vpnavg;
extern const PRM_ATTR_T pndef_PnED0;
extern const PRM_ATTR_T pndef_PnED1;
extern const PRM_ATTR_T pndef_PnED3;
extern const PRM_ATTR_T pndef_PnED4;
extern const PRM_ATTR_T pndef_PnED5;
extern const PRM_ATTR_T pndef_PnED6;
extern const PRM_ATTR_T pndef_PnED7;
extern const PRM_ATTR_T pndef_PnED8;
extern const PRM_ATTR_T pndef_PnED9;
extern const PRM_ATTR_T pndef_TunLessGain;
extern const PRM_ATTR_T pndef_TunLessStiff;
extern const PRM_ATTR_T pndef_TunLessTrqLpf;
extern const PRM_ATTR_T pndef_TunLessVfbLpf;
extern const PRM_ATTR_T pndef_MLess_vrat_max;
extern const PRM_ATTR_T pndef_MLess_oslv_pol;
extern const PRM_ATTR_T pndef_Mless_jmot;
extern const PRM_ATTR_T pndef_Mless_exp_spd_jmot;
extern const PRM_ATTR_T pndef_FanLockTime;
extern const PRM_ATTR_T pndef_SvonFanLock_Time;
extern const PRM_ATTR_T pndef_dq_sw_L;
extern const PRM_ATTR_T pndef_v1max_L;
extern const PRM_ATTR_T pndef_TunLessGain_L;
extern const PRM_ATTR_T pndef_TunLessStiff_L;
extern const PRM_ATTR_T pndef_DcBaseLvl;
extern const PRM_ATTR_T pndef_typm_e;
extern const PRM_ATTR_T pndef_encver;
extern const PRM_ATTR_T pndef_motw;
extern const PRM_ATTR_T pndef_bit_dp;
extern const PRM_ATTR_T pndef_limmlt;
extern const PRM_ATTR_T pndef_vrat_max;
extern const PRM_ATTR_T pndef_oslv_pol;
extern const PRM_ATTR_T pndef_rattrq;
extern const PRM_ATTR_T pndef_maxtrq;
extern const PRM_ATTR_T pndef_irat;
extern const PRM_ATTR_T pndef_imax;
extern const PRM_ATTR_T pndef_emfcmp;
extern const PRM_ATTR_T pndef_jmot;
extern const PRM_ATTR_T pndef_motr;
extern const PRM_ATTR_T pndef_motl;
extern const PRM_ATTR_T pndef_bastrq;
extern const PRM_ATTR_T pndef_midtrq;
extern const PRM_ATTR_T pndef_midolt;
extern const PRM_ATTR_T pndef_mtrq2;
extern const PRM_ATTR_T pndef_mtolt2;
extern const PRM_ATTR_T pndef_phscmpM1;
extern const PRM_ATTR_T pndef_ppitch;
extern const PRM_ATTR_T pndef_motlq0;
extern const PRM_ATTR_T pndef_motlq1;
extern const PRM_ATTR_T pndef_exp_trat_jmot;
extern const PRM_ATTR_T pndef_exp_motw_spd;
extern const PRM_ATTR_T pndef_BitnoDpoint;
extern const PRM_ATTR_T pndef_poleoffset;
extern const PRM_ATTR_T pndef_motresfrq;
extern const PRM_ATTR_T pndef_phscmpM2;
extern const PRM_ATTR_T pndef_flg_wf;
extern const PRM_ATTR_T pndef_motfuncf;
extern const PRM_ATTR_T pndef_mot_pwmf;
extern const PRM_ATTR_T pndef_mot_ibs;
extern const PRM_ATTR_T pndef_mot_imd;
extern const PRM_ATTR_T pndef_mot_imdt;
extern const PRM_ATTR_T pndef_mot_imd2;
extern const PRM_ATTR_T pndef_mot_imdt2;
extern const PRM_ATTR_T pndef_mot_ichgspd;
extern const PRM_ATTR_T pndef_mot_oc;
extern const PRM_ATTR_T pndef_mot_TunLess;
extern const PRM_ATTR_T pndef_RefAdjust_ForWF;
extern const PRM_ATTR_T pndef_LCompForWF;
extern const PRM_ATTR_T pndef_RegPhscmp;
extern const PRM_ATTR_T pndef_RippleCompGainE;
extern const PRM_ATTR_T pndef_RippleCompCycle1;
extern const PRM_ATTR_T pndef_RippleCompAmplitude1;
extern const PRM_ATTR_T pndef_RippleCompPhase1;
extern const PRM_ATTR_T pndef_RippleCompCycle2;
extern const PRM_ATTR_T pndef_RippleCompAmplitude2;
extern const PRM_ATTR_T pndef_RippleCompPhase2;
extern const PRM_ATTR_T pndef_RippleCompCycle3;
extern const PRM_ATTR_T pndef_RippleCompAmplitude3;
extern const PRM_ATTR_T pndef_RippleCompPhase3;
extern const PRM_ATTR_T pndef_RippleCompCycle4;
extern const PRM_ATTR_T pndef_RippleCompAmplitude4;
extern const PRM_ATTR_T pndef_RippleCompPhase4;
extern const PRM_ATTR_T pndef_RippleCompCycle5;
extern const PRM_ATTR_T pndef_RippleCompAmplitude5;
extern const PRM_ATTR_T pndef_RippleCompPhase5;
extern const PRM_ATTR_T pndef_RippleCompCycle6;
extern const PRM_ATTR_T pndef_RippleCompAmplitude6;
extern const PRM_ATTR_T pndef_RippleCompPhase6;
extern const PRM_ATTR_T pndef_RippleCompPhase6;





/******************************* End of File *******************************/
#endif //__PN_PRM_DEF_TBL_H