;/****************************************************************************************************/
;/*																									*/
;/*																									*/
;/*		init_SGD7.s : MercurySoCブートプログラム													*/
;/*																									*/
;/*																									*/
;/****************************************************************************************************/
;/*																									*/
;/*																									*/
;/*																									*/
;/*																									*/
;/*																									*/
;/*																									*/
;/*																									*/
;/*																									*/
;/*																									*/
;/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
;/*																									*/
;/*		Rev.1.00 : 2013.07.13  Y.Tanaka		初版													*/
;/*																									*/
;/****************************************************************************************************/
	CODE32
	REQUIRE8
	PRESERVE8

;/****************************************************************************************************/
;/*																									*/
;/*		定数定義																					*/
;/*																									*/
;/****************************************************************************************************/
PRE_STACK_BASE	EQU	0x68080000
MODE_USR        EQU 0x10
MODE_FIQ        EQU 0x11
MODE_IRQ        EQU 0x12
MODE_SVC        EQU 0x13
MODE_ABT        EQU 0x17
MODE_UND        EQU 0x1B
MODE_SYS        EQU 0x1F

;------ Interrput enable ------
FIQ_DISABLE EQU 0x40
IRQ_DISABLE EQU 0x80

;============ Define for Stack ==========
;------ Stack Size ------
USR_STACK_SIZE  EQU  0x400
FIQ_STACK_SIZE  EQU  0
IRQ_STACK_SIZE  EQU  0x400
SVC_STACK_SIZE  EQU  0
ABT_STACK_SIZE  EQU  0
UND_STACK_SIZE  EQU  0
SYS_STACK_SIZE  EQU  0

;------ Stack Offset ------
USR_STACK_OFFSET    EQU 0
FIQ_STACK_OFFSET    EQU USR_STACK_OFFSET + USR_STACK_SIZE
IRQ_STACK_OFFSET    EQU FIQ_STACK_OFFSET + FIQ_STACK_SIZE
SVC_STACK_OFFSET    EQU IRQ_STACK_OFFSET + IRQ_STACK_SIZE
ABT_STACK_OFFSET    EQU SVC_STACK_OFFSET + SVC_STACK_SIZE
UND_STACK_OFFSET    EQU ABT_STACK_OFFSET + ABT_STACK_SIZE
SYS_STACK_OFFSET    EQU UND_STACK_OFFSET + UND_STACK_SIZE

;/***************************************************************************************************/
;/*																									*/
;/*		Scatter file region name import																*/
;/*																									*/
;/***************************************************************************************************/
;/*-------------------------------------------------------------------------------------------------*/
;/*		関連定義（ROM->RAM展開用）																	*/
;/*-------------------------------------------------------------------------------------------------*/
ARMPF_VIC_BASE			EQU		0xEFFDF000
ARMPF_VIC_UEN			EQU		ARMPF_VIC_BASE + 0x1C8

;/***************************************************************************************************/
;/*																									*/
;/*		function import																				*/
;/*																									*/
;/***************************************************************************************************/
;	EXPORT  Undefined_Handler
;	EXPORT  Prefetch_Handler
;	EXPORT  Abort_Handler
;	EXPORT  FIQ_Handler
;	EXPORT  IRQ_Handler


;/***************************************************************************************************/
;/*																									*/
;/*		割り込み入力時処理																			*/
;/*																									*/
;/***************************************************************************************************/
;/*-------------------------------------------------------------------------------------------------*/
;/* 関数インポート																					*/
;/*-------------------------------------------------------------------------------------------------*/
	IMPORT	Common_IRQHandler0
	IMPORT	Common_IRQHandler1
	IMPORT	ScanAIntHandler
	IMPORT	ScanBIntHandler
	IMPORT	ScanCIntHandler
;/*-------------------------------------------------------------------------------------------------*/
;/* 関数エクスポート																				*/
;/*-------------------------------------------------------------------------------------------------*/
	EXPORT	__SET_VE
	EXPORT	IRQHandler0
	EXPORT	IRQHandler1
	EXPORT	ScanAHandler
	EXPORT	ScanBHandler
	EXPORT	ScanCHandler
	EXPORT	__EI
	EXPORT	__DI

	AREA INIT_VECT_CODE, CODE, READONLY
;/*-------------------------------------------------------------------------------------------------*/
;/* VIC VEbitセット																					*/
;/*-------------------------------------------------------------------------------------------------*/
;/* CP15 の設定により、VIC インターフェース設定を有効にする											*/
;/* (システム制御レジスタ(c1　24bit)VE　bitをセット)												*/
__SET_VE	FUNCTION
		PUSH	{r0,lr}
	    MRC p15, 0, r0, c1, c0, 0
	    ORR r0, r0, #(0x1 << 24)
	    MCR p15, 0, r0, c1, c0, 0
		POP	{r0,pc}
	ENDFUNC

	AREA VECT_CODE, CODE, READONLY
;/*-------------------------------------------------------------------------------------------------*/
;/* レジスタ退避＆要因ハンドラジャンプ for ScanA													*/
;/*-------------------------------------------------------------------------------------------------*/
ScanAHandler
;動作確認IRQレジスタ退避処理
		PUSH    {r0-r12,lr}				;割り込み前のレジスタ,lr_irqを退避
		VPUSH   {d0-d15}				;割り込み前のFPUレジスタを退避	<S151> VFP(FPU)レジスタ処理追加

		MRS		r12, SPSR				;SPSR_irqをr12にコピー
		PUSH	{r12}					;r12をプッシュ
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYSモードに切り替え。IRQディセーブル
										;ここからユーザモードのレジスタ。
		AND		R1, r14, #4				;スタックの整列をテス							<S0E3>
		SUB		sp, sp, R1				;スタック調整									<S0E3>
		PUSH	{R1, lr}				;スタック調整値、割り込み前のlr_usrを退避		<S0E3>
		BL		ScanAIntHandler			;ScanAハンドラへジャンプ
		POP		{R1, lr}				;スタック調整値、割り込み前のlr_usrを復帰		<S0E3>
		ADD		sp, sp, R1				;スタック調整を復元								<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQモードに切り替え。IRQディセーブル
										;ここからIRQモードのレジスタ。
		POP		{r12};	MSR				SPSR_cf, r12	;SPSR_irqを復帰					<S0E3>
		MSR		SPSR_cxsf, r12			;SPSR_irqを復帰	<Ando>

		VPOP   {d0-d15}					;割り込み前のFPUレジスタを復帰	<S151> VFP(FPU)レジスタ処理追加
		POP		{r0-r12,lr}				;割り込み前のレジスタ,lr_irqの復帰
		SUBS	pc, lr, #4				;IRQからの復帰アドレス計算


;/*-------------------------------------------------------------------------------------------------*/
;/* レジスタ退避＆要因ハンドラジャンプ for ScanB													*/
;/*-------------------------------------------------------------------------------------------------*/
ScanBHandler
;動作確認IRQレジスタ退避処理
		PUSH    {r0-r12,lr}				;割り込み前のレジスタ,lr_irqを退避
		VPUSH   {d0-d15}				;割り込み前のFPUレジスタを退避	<S151> VFP(FPU)レジスタ処理追加

		MRS		r12, SPSR				;SPSR_irqをr12にコピー
		PUSH	{r12}					;r12をプッシュ
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYSモードに切り替え。IRQディセーブル
										;ここからユーザモードのレジスタ。
		AND		R1, r14, #4				;スタックの整列をテスト							<S0E3>
		SUB		sp, sp, R1				;スタック調整									<S0E3>
		PUSH	{R1, lr}				;スタック調整値、割り込み前のlr_usrを退避		<S0E3>
		BL		ScanBIntHandler			;ScanBハンドラへジャンプ
		POP		{R1, lr}				;スタック調整値、割り込み前のlr_usrを復帰		<S0E3>
		ADD		sp, sp, R1				;スタック調整を復元								<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQモードに切り替え。IRQディセーブル
										;ここからIRQモードのレジスタ。
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irqを復帰	<Ando>
		MSR		SPSR_cxsf, r12			;SPSR_irqを復帰	<Ando>

		VPOP   {d0-d15}					;割り込み前のFPUレジスタを復帰	<S151> VFP(FPU)レジスタ処理追加
		POP		{r0-r12,lr}				;割り込み前のレジスタ,lr_irqの復帰
		SUBS	pc, lr, #4				;IRQからの復帰アドレス計算


;/*-------------------------------------------------------------------------------------------------*/
;/* レジスタ退避＆要因ハンドラジャンプ for ScanC													*/
;/*-------------------------------------------------------------------------------------------------*/
ScanCHandler
;動作確認IRQレジスタ退避処理
		PUSH    {r0-r12,lr}				;割り込み前のレジスタ,lr_irqを退避
		VPUSH   {d0-d15}				;割り込み前のFPUレジスタを退避	<S151> VFP(FPU)レジスタ処理追加

		MRS		r12, SPSR				;SPSR_irqをr12にコピー
		PUSH	{r12}					;r12をプッシュ
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYSモードに切り替え。IRQディセーブル
										;ここからユーザモードのレジスタ。
		AND		R1, r14, #4				;スタックの整列をテスト							<S0E3>
		SUB		sp, sp, R1				;スタック調整									<S0E3>
		PUSH		{R1, lr}			;スタック調整値、割り込み前のlr_usrを退避		<S0E3>
		BL		ScanCIntHandler			;ScanCハンドラへジャンプ
		POP		{R1, lr}				;スタック調整値、割り込み前のlr_usrを復帰		<S0E3>
		ADD		sp, sp, R1				;スタック調整を復元								<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQモードに切り替え。IRQディセーブル
										;ここからIRQモードのレジスタ。
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irqを復帰	<Ando>
		MSR		SPSR_cxsf, r12			;SPSR_irqを復帰	<Ando>

		VPOP   {d0-d15}					;割り込み前のFPUレジスタを復帰	<S151> VFP(FPU)レジスタ処理追加
		POP		{r0-r12,lr}				;割り込み前のレジスタ,lr_irqの復帰
		SUBS	pc, lr, #4				;IRQからの復帰アドレス計算


;/*-------------------------------------------------------------------------------------------------*/
;/* レジスタ退避＆要因ハンドラジャンプ for ARMPF_VIC_ISC0											*/
;/*-------------------------------------------------------------------------------------------------*/
IRQHandler0
;動作確認IRQレジスタ退避処理
		PUSH    {r0-r12,lr}				;割り込み前のレジスタ,lr_irqを退避
		VPUSH   {d0-d15}				;割り込み前のFPUレジスタを退避	<S151> VFP(FPU)レジスタ処理追加

		MRS		r12, SPSR					;SPSR_irqをr12にコピー
		PUSH	{r12}					;r12をプッシュ
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYSモードに切り替え。IRQディセーブル
										;ここからユーザモードのレジスタ。
		AND		R1, r14, #4				;スタックの整列をテスト						<S0E3>
		SUB		sp, sp, R1				;スタック調整								<S0E3>
		PUSH	{R1, lr}				;スタック調整値、割り込み前のlr_usrを退避	<S0E3>
		BL		Common_IRQHandler0		;IRQハンドラへジャンプ
		POP		{R1, lr}				;スタック調整値、割り込み前のlr_usrを復帰	<S0E3>
		ADD		sp, sp, R1				;スタック調整を復元							<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQモードに切り替え。IRQディセーブル
										;ここからIRQモードのレジスタ。
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irqを復帰								<S0E3>
		MSR		SPSR_cxsf, r12			;SPSR_irqを復帰								<S0E3>

		VPOP   {d0-d15}					;割り込み前のFPUレジスタを復帰	<S151> VFP(FPU)レジスタ処理追加
		POP		{r0-r12,lr}				;割り込み前のレジスタ,lr_irqの復帰
		SUBS			pc, lr, #4		;IRQからの復帰アドレス計算


;/*-------------------------------------------------------------------------------------------------*/
;/* レジスタ退避＆要因ハンドラジャンプ for ARMPF_VIC_ISC1											*/
;/*-------------------------------------------------------------------------------------------------*/
IRQHandler1
;動作確認IRQレジスタ退避処理
		PUSH    {r0-r12,lr}				;割り込み前のレジスタ,lr_irqを退避
		VPUSH   {d0-d15}				;割り込み前のFPUレジスタを退避	<S151> VFP(FPU)レジスタ処理追加

		MRS		r12, SPSR				;SPSR_irqをr12にコピー
		PUSH	{r12}					;r12をプッシュ
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYSモードに切り替え。IRQディセーブル
										;ここからユーザモードのレジスタ。
		AND		R1, r14, #4				;スタックの整列をテスト						<S0E3>
		SUB		sp, sp, R1				;スタック調整								<S0E3>
		PUSH	{R1, lr}				;スタック調整値、割り込み前のlr_usrを退避	<S0E3>
		BL		Common_IRQHandler1		;IRQハンドラへジャンプ
		POP		{R1, lr}				;スタック調整値、割り込み前のlr_usrを復帰	<S0E3>
		ADD		sp, sp, R1				;スタック調整を復元							<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQモードに切り替え。IRQディセーブル
										;ここからIRQモードのレジスタ。
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irqを復帰
		MSR		SPSR_cxsf, r12			;SPSR_irqを復帰

		VPOP   {d0-d15}					;割り込み前のFPUレジスタを復帰	<S151> VFP(FPU)レジスタ処理追加
		POP		{r0-r12,lr}				;割り込み前のレジスタ,lr_irqの復帰
		SUBS	pc, lr, #4				;IRQからの復帰アドレス計算


;/*-------------------------------------------------------------------------------------------------*/
;/* 割り込みイネーブル																				*/
;/*-------------------------------------------------------------------------------------------------*/
__EI	FUNCTION
		PUSH	{r0,lr}
		MRS		r0, CPSR;		/* st <- CPSR */
		BIC		r0, r0, #0x80;		/* clear interrupt enable bit */
		MSR		CPSR_c, r0;		/* CPSR <- st */
		POP		{r0,PC}
	ENDFUNC


;/*-------------------------------------------------------------------------------------------------*/
;/* 割り込みディセーブル																				*/
;/*-------------------------------------------------------------------------------------------------*/
__DI	FUNCTION
		PUSH	{r0,lr}
		MRS r0, CPSR;		/* st <- CPSR				*/
		ORR r0, r0, #0x80;	/* set interrupt enable bit	*/
		MSR CPSR_c, r0;		/* CPSR <- st				*/
		POP		{r0,PC}
	ENDFUNC


		END
;/*-------end of file----------------------------------------------------------------------------------------*/
