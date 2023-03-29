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
	AREA    Start,CODE,READONLY


	ENTRY
	IMPORT EFW_Loader									;<S146>
;	IMPORT MercuryMain
MercuryMain	EQU 0x68000001								;<S146>

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
USR_STACK_SIZE  EQU  0x100	;<S01E>
FIQ_STACK_SIZE  EQU  0
IRQ_STACK_SIZE  EQU  0xA00	;<S01E><S127>
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
;/*		Load region																					*/
;/*-------------------------------------------------------------------------------------------------*/
;	IMPORT |Load$$LR$$LOAD_RUNTIME$$Base|				;0xFFF28000
;	IMPORT |Load$$B0TCM$$Base|				;0xFFF28000
;	IMPORT |Load$$B1TCM$$Base|
;	IMPORT |Load$$ATCM$$Base|
;	IMPORT |Load$$AXIRAM_CODE$$Base|
;	IMPORT |Load$$AXIRAM_DATA$$Base|
	IMPORT |Load$$ELFLOAD_CODE$$Base|					;0x6806C000 <S146> ランタイムロード時のみ使用
;/*-------------------------------------------------------------------------------------------------*/
;/*		Image region(Execute region)																*/
;/*-------------------------------------------------------------------------------------------------*/
;	IMPORT |Image$$B0TCM$$Base|							;0x67FA0000
;	IMPORT |Image$$B0TCM$$Length|
;;	IMPORT |Image$$B0TCM$$RO$$Length|
;;	IMPORT |Image$$B0TCM$$RW$$Length|
;	IMPORT |Image$$B0TCM$$Limit|
;	IMPORT |Image$$B0TCM$$ZI$$Limit|
;	IMPORT |Image$$ARM_LIB_STACKHEAP$$Limit|			;0x67FAF800
;	IMPORT |Image$$B1TCM$$Base|							;0x67FB8000
;	IMPORT |Image$$B1TCM$$Length|
;	IMPORT |Image$$B1TCM$$Limit|
;	IMPORT |Image$$ATCM$$Base|							;0x67FC0000
;	IMPORT |Image$$ATCM$$Length|
;	IMPORT |Image$$ATCM$$Limit|
;	IMPORT |Image$$AXIRAM_CODE$$Base|					;0x68000000
;	IMPORT |Image$$AXIRAM_CODE$$Length|
;	IMPORT |Image$$AXIRAM_CODE$$Limit|
;	IMPORT |Image$$AXIRAM_DATA$$Base|					;0x68060000
;	IMPORT |Image$$AXIRAM_DATA$$Length|
;	IMPORT |Image$$AXIRAM_DATA$$Limit|

;; <S146> >>>>>
	IMPORT |Image$$ELFLOAD_DATA$$Base|					;0x6806C000
;	IMPORT |Image$$ELFLOAD_DATA$$Length|
	IMPORT |Image$$ELFLOAD_DATA$$Limit|
	IMPORT |Image$$ELFLOAD_CODE$$Base|					;0x68078000
;	IMPORT |Image$$ELFLOAD_CODE$$Length|
	IMPORT |Image$$ELFLOAD_CODE$$Limit|
;; <<<<< <S146>

;/*-------------------------------------------------------------------------------------------------*/
;/*		関連定義（ROM->RAM展開用）																	*/
;/*-------------------------------------------------------------------------------------------------*/
B0TCM_LOAD_END			EQU		0x67FB0000
B1TCM_LOAD_END			EQU		0x67FC0000
ATCM_LOAD_END			EQU		0x67FF0000
AXIRAM_CODE_LOAD_END	EQU		0x68060000
AXIRAM_DATA_LOAD_END	EQU		0x68080000

ARMPF_VIC_BASE			EQU		0xEFFDF000
ARMPF_VIC_UEN			EQU		ARMPF_VIC_BASE + 0x1C8

;/***************************************************************************************************/
;/*																									*/
;/*		function import																				*/
;/*																									*/
;/***************************************************************************************************/
;	IMPORT  Undefined_Handler
;	IMPORT  Prefetch_Handler
;	IMPORT  Abort_Handler
;	IMPORT  FIQ_Handler
;	IMPORT  IRQ_Handler


	EXPORT	reset
;/***************************************************************************************************/
;/*																									*/
;/*		vector base																					*/
;/*																									*/
;/***************************************************************************************************/
reset           B   Initialize					; 0x0000
undef           B   Undefined_Handler			; 0x0004
svc             B   SWI_Handler					; 0x0008
pabt            B   Prefetch_Handler			; 0x000C
dabt            B   Abort_Handler				; 0x0010
                NOP								; 0x0014（reserved）
;irq             B   IRQ_Handler				; 0x0018 VIC使用時は不要
                NOP								; 0x0018
fiq             B   FIQ_Handler					; 0x0020

	EXPORT	Initialize
;/***************************************************************************************************/
;/*																									*/
;/*		スタートアップ処理																			*/
;/*																									*/
;/***************************************************************************************************/
;; -----------------------------------------------------------------------------
;; CPU-SS の初期化(例)
;;  汎用レジスタ, TCM, MPU, CRS, FRU, CAHCE 等
;;  ARMのテクニカル･リファレンス･マニュアルを参照して下さい.
;; -----------------------------------------------------------------------------
Initialize
;			/* Chenge SUPERVISOE mode and initialize sp and lr */
			msr     cpsr_c, #(MODE_SVC :OR: FIQ_DISABLE :OR: IRQ_DISABLE)
			ldr     sp, =PRE_STACK_BASE								;/* stack pointer[r13] */
			ldr     lr, =0x00000000									;/* link pointer[r14]  */

; *-------------------------------------------------------------------------------------------------
; *
; *  Setup for memory controller
; *
; *-------------------------------------------------------------------------------------------------
; */
			MOV r0,#0
			MOV r1,#0
			MOV r2,#0
			MOV r3,#0
			MOV r4,#0
			MOV r5,#0
			MOV r6,#0
			MOV r7,#0
			MOV r8,#0
			MOV r9,#0
			MOV r10,#0
			MOV r11,#0
			MOV r12,#0
			MOV r14,#0

; *-------------------------------------------------------------------------------------------------
; *
; *  Initialize registers (only set svc stack pointer to initialize memory controller)
; *
; *-------------------------------------------------------------------------------------------------
; */

        
            ;; Setup RSP & Stack Pointer 
            ;;==========================
;			LDR r0, =|Image$$ARM_LIB_STACKHEAP$$Limit|
			LDR r0, =0x67FAF500

			;;------ FIQ mode ------
			MSR CPSR_c, #MODE_FIQ :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			MOV r8,#0
			MOV r9,#0
			MOV r10,#0
			MOV r11,#0
			MOV r12,#0
			MOV r14,#0
			ADD r1, r0, #FIQ_STACK_OFFSET
			MOV sp, r1

			;;------ IRQ mode ------
			MSR CPSR_c, #MODE_IRQ :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			MOV r12,#0
			MOV r14,#0
			ADD r1, r0, #IRQ_STACK_OFFSET
			MOV sp, r1

			;;------ Abort mode ------
			MSR CPSR_c, #MODE_ABT :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			MOV r12,#0
			MOV r14,#0
			ADD r1, r0, #ABT_STACK_OFFSET
			MOV sp, r1

			;;------ Undefine mode ------
			MSR CPSR_c, #MODE_UND :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			MOV r12,#0
			MOV r14,#0
			ADD r1, r0, #UND_STACK_OFFSET
			MOV sp, r1

			;;------ System mode ------
			MSR CPSR_c, #MODE_SYS :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			MOV r12,#0
			MOV r14,#0
			ADD r1, r0, #SYS_STACK_OFFSET
			MOV sp, r1

			;;------ Supervisor mode ------
			MSR CPSR_c, #MODE_SVC :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			MOV r12,#0
			MOV r14,#0
			ADD r1, r0, #SVC_STACK_OFFSET
			MOV sp, r1

;/* VICのUENをここで立てる */
			LDR r4,=ARMPF_VIC_UEN
			MOV r0, #1
			STR r0, [r4, #0]

;; -----------------------------------------------------------------------------
;; 実行モードの設定
;;  以降, このモードで実行します.
;; -----------------------------------------------------------------------------
			;;------ System mode again ------
			MSR CPSR_c, #MODE_SYS :OR: FIQ_DISABLE :OR: IRQ_DISABLE
			NOP
			NOP
			NOP
			NOP

;; -----------------------------------------------------------------------------
;; TCMの初期化
;;  ATCM, BTCMのベースアドレスを設定し, TCMを有効化.
;; -----------------------------------------------------------------------------
            ;;------ enable TCM ------
;;          LDR r4,=0x67FC0045      ;ATCM(BaseAdr=0x67FC_0000, 256KB, En)							/* <S0E2> */
;;          LDR r5,=0x67FA0041      ;BTCM(BaseAdr=0x67FA_0000, 128KB, En)							/* <S0E2> */
            LDR r4,=0x67FC0025      ;ATCM(BaseAdr=0x67FC_0000, 256KB, En)							/* <S0E2> */
            LDR r5,=0x67FA0021      ;BTCM(BaseAdr=0x67FA_0000, 128KB, En)							/* <S0E2> */
            MCR p15,0,r4,c9,c1,1    ;Write ATCM Region Register
            MCR p15,0,r5,c9,c1,0    ;Write BTCM Region Register

            NOP
            NOP


;; -----------------------------------------------------------------------------
;; MPUの初期化
;;  [メモリタイプの設定]
;;      Region.1  ハイベクタブート領域 		ノーマルメモリ, 共有, 実行可
;;      Region.2  B0TCM領域(Data)		ノーマルメモリ, 共有, 実行不可
;;      Region.3  B1TCM領域(Code)		ノーマルメモリ, 共有, 実行可
;;      Region.4  ATCM領域(Code)			ノーマルメモリ, 共有, 実行可
;;      Region.5  AXIRAM_CODE領域(Code)	ノーマルメモリ, 共有, 実行可
;;      Region.6  AXIRAM_DATA領域(Data)	ノーマルメモリ, 共有, 実行不可
;;      Region.0  全領域(4GB)				デバイス, 共有, 実行不可
;; -----------------------------------------------------------------------------

            ;;------ initialize MPU ------
            ;; -------------------------------------------------------------------------------------
            ;; Region No.1 : High Vector Boot
            ;; -------------------------------------------------------------------------------------
            LDR r4,=0x00000001  ;; MPU Memory Region Number Register  : [ 3:0] Region = 1h
            LDR r5,=0xFFFF0000  ;; MPU Region Base Address Register   : [31:5] Base Address = FFFF0000h
           LDR r6,=0x00000035  ;; MPU Region Size and Enable         : [ 5:1] Region Size = 11010b (128MB)			/* <S0E2> */
;<暫定>コメント化            LDR r6,=0x0000001f  ;; MPU Region Size and Enable         : [ 5:1] Region Size = 00101b (64KB)			/* <S0E2> */
;                                                                     : [   0] Enable      = 1b     (Enable)
            LDR r7,=0x0000030C  ;; MPU Region Access Control Register : [  12] XN  = 0b   (All Instruction Fetch Enabled)
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 001b (*)
;                                                                     : [   2] S   = 1b   (Non-Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : * TEX,C,B=001_0_0 (Outer & Inner Non-Cacheable)

            MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
            MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
            MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
            MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register


            ;; -------------------------------------------------------------------------------------
            ;; Region No.2 : B0TCM Data (<----CPU)
            ;; -------------------------------------------------------------------------------------
            LDR r4,=0x00000002  ;; MPU Memory Region Number Register  : [ 3:0] Region = 2h
            LDR r5,=0x67FA0000  ;; MPU Region Base Address Register   : [31:5] Base Address = 30020000h
            LDR r6,=0x0000001F  ;; MPU Region Size and Enable         : [ 5:1] Region Size = 01111b (64KB)
;                                                                     : [   0] Enable      = 1b     (Enable)
            LDR r7,=0x0000130c  ;; MPU Region Access Control Register : [  12] XN  = 1b   (All Instruction Fetch Disabled)
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 001b (*)
;                                                                     : [   2] S   = 1b   (Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : * TEX,C,B=001_0_0 (Outer & Inner Non-Cacheable)

            MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
            MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
            MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
            MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register


            ;; -------------------------------------------------------------------------------------
            ;; Region No.3 : B1TCM Code (<----CPU)
            ;; -------------------------------------------------------------------------------------
            LDR r4,=0x00000003  ;; MPU Memory Region Number Register  : [ 3:0] Region = 3h
            LDR r5,=0x67FB8000  ;; MPU Region Base Address Register   : [31:5] Base Address = 30038000h
            LDR r6,=0x0000001D  ;; MPU Region Size and Enable         : [ 5:1] Region Size =  01110b (32KB)
;                                                                     : [   0] Enable      = 1b     (Enable)
            LDR r7,=0x0000030c  ;; MPU Region Access Control Register : [  12] XN  = 0b   (All Instruction Fetch Enabled)
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 001b (*)
;                                                                     : [   2] S   = 1b   (Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : * TEX,C,B=001_0_0 (Outer & Inner Non-Cacheable)

            MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
            MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
            MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
            MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register


            ;; -------------------------------------------------------------------------------------
            ;; Region No.4 : ATCM Code (<----CPU)
            ;; -------------------------------------------------------------------------------------
            LDR r4,=0x00000004  ;; MPU Memory Region Number Register  : [ 3:0] Region = 4h
            LDR r5,=0x67FC0000  ;; MPU Region Base Address Register   : [31:5] Base Address = 30040000h
            LDR r6,=0x00000023 ;; MPU Region Size and Enable         : [ 5:1] Region Size =  10001b (256KB)
;                                                                     : [   0] Enable      = 1b     (Enable)
            LDR r7,=0x0000030c  ;; MPU Region Access Control Register : [  12] XN  = 0b   (All Instruction Fetch Enabled)
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 001b (*)
;                                                                     : [   2] S   = 1b   (Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : * TEX,C,B=001_0_0 (Outer & Inner Non-Cacheable)

            MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
            MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
            MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
            MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register


            ;; -------------------------------------------------------------------------------------
            ;; Region No.5 : AXIRAM_CODE (<----CPU)
            ;; -------------------------------------------------------------------------------------
            LDR r4,=0x00000005  ;; MPU Memory Region Number Register  : [ 3:0] Region = 6h
            LDR r5,=0x68000000  ;; MPU Region Base Address Register   : [31:5] Base Address = 68040000h
            LDR r6,=0x00000023  ;; MPU Region Size and Enable         : [ 5:1] Region Size = 10001b (256KB)
;                                                                     : [   0] Enable      = 1b     (Enable)
            LDR r7,=0x0000030c  ;; MPU Region Access Control Register : [  12] XN  = 0b   (All Instruction Fetch Enabled)
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 001b (*)
;                                                                     : [   2] S   = 1b   (Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : * TEX,C,B=001_0_0 (Outer & Inner Non-Cacheable)

            MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
            MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
            MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
            MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register


		;; -------------------------------------------------------------------------------------
		;; Region No.6 : AXIRAM_DATA (<----CPU)
		;; -------------------------------------------------------------------------------------
		LDR r4,=0x00000006  ;; MPU Memory Region Number Register  : [ 3:0] Region = 5h
		LDR r5,=0x68060000  ;; MPU Region Base Address Register   : [31:5] Base Address = 68000000h
		LDR r6,=0x00000023  ;; MPU Region Size and Enable         : [ 5:1] Region Size = 10001b (256KB)
;                                                        : [   0] Enable      = 1b     (Enable)
;<S146> >>>>> 展開処理をRAMで実行するため、ここではフェッチ有効としておき、展開処理終了後に無効とする
;		LDR r7,=0x0000130c  ;; MPU Region Access Control Register : [  12] XN  = 1b   (All Instruction Fetch Disabled)
		LDR r7,=0x0000030c  ;; MPU Region Access Control Register : [  12] XN  = 0b   (All Instruction Fetch Enabled)
;<<<<< <S146>
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 001b (*)
;                                                                     : [   2] S   = 1b   (Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : * TEX,C,B=001_0_0 (Outer & Inner Non-Cacheable)

		MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
		MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
		MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
		MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register


		;; -------------------------------------------------------------------------------------
		;; Region No.0 : Back-ground
		;; -------------------------------------------------------------------------------------
		LDR r4,=0x00000000  ;; MPU Memory Region Number Register  : [ 3:0] Region = 0h
		LDR r5,=0x00000000  ;; MPU Region Base Address Register   : [31:5] Base Address = 00000000h
		LDR r6,=0x0000003F  ;; MPU Region Size and Enable         : [ 5:1] Region Size = 11111b (4GB)
;                                                                     : [   0] Enable      = 1b     (Enable)
;		LDR r7,=0x00001304  ;; MPU Region Access Control Register : [  12] XN  = 1b   (All Instruction Fetch Disabled)	/* <S0E2> */
		LDR r7,=0x00001305  ;; MPU Region Access Control Register : [  12] XN  = 1b   (All Instruction Fetch Disabled)	/* <S0E2> */
;                                                                     : [10:8] AP  = 011b (Full Access)
;                                                                     : [ 5:3] TEX = 000b (*)
;                                                                     : [   2] S   = 1b   (Shared)
;                                                                     : [   1] C   = 0b   (Non-Cacheable*)
;                                                                     : [   0] B   = 0b   (Non-Bufferable*)
;                                                                     : [   0] B   = 1b   (Bufferable*)
;                                                                     : * TEX,C,B=000_0_0 (Strongry-Ordered)
;                                                                     : * TEX,C,B=000_0_1 (Shared-Device)

		MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
		MCR     p15, 0, r5, c6, c1, 0   ;; Write MPU Region Base Address Register
		MCR     p15, 0, r6, c6, c1, 2   ;; Write MPU Region Size and Enable
		MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register



;/* 定義名・コメント見直し <S0E3>	*/
VE			EQU		(1 << 24)		;/* VICコントローラ */
FI			EQU		(1 << 21)		;/*  */
ICache			EQU		(1 << 12)		;/* 命令キャッシュ					<S0E3> */
DCache			EQU		(1 << 2)		;/* データキャッシュ				<S0E3> */
AlignChk		EQU		(1 << 1)		;/* アライメントフォルトチェック	<S0E3> */
MPU			EQU		(1 << 0)		;/* MPU */

		MRC     p15, 0, r6, c1, c0, 0
;		LDR     r7, =VE :OR: ICache :OR: DCache :OR: MPU
;		LDR     r7, =VE :OR: MPU
		LDR     r7, =MPU
		ORR     r6, r6, r7
;		BIC	r6,	#FI
		DSB
		MCR     p15, 0, r6, c1, c0, 0
		ISB


;;------ initialize CRS ------

;;------ initialize FPU ------
;-----------------------------------------------------------------
; Enable access to NEON/VFP by enabling access to Coprocessors 10 and 11.
; Enables Full Access i.e. in both priv and non priv modes
;-----------------------------------------------------------------
		MRC     p15, 0, r0, c1, c0, 2      ; read CP access register
		ORR     r0, r0, #(0x3  <<20)       ; enable access CP 10
		ORR     r0, r0, #(0x3  <<22)       ; enable access CP 11        
		MCR     p15, 0, r0, c1, c0, 2      ; write CP access register back  

;110222,tanaka21 FPU未使用時はコメントアウトすること
;-----------------------------------------------------------------
; Switch on the VFP and NEON Hardware
;-----------------------------------------------------------------
		MOV     r0, #0                      ; Set up a register
		ORR     r0, r0, #(0x1 << 30)
		VMSR    FPEXC, r0                   ; Write FPEXC register, EN bit set


;-----------------------------------------------------------------
; reset VFP register	<S151> VFP(FPU)レジスタ処理追加
;-----------------------------------------------------------------
		VLDR.F64 d0,  =0
		VLDR.F64 d1,  =0
		VLDR.F64 d2,  =0
		VLDR.F64 d3,  =0
		VLDR.F64 d4,  =0
		VLDR.F64 d5,  =0
		VLDR.F64 d6,  =0
		VLDR.F64 d7,  =0
		VLDR.F64 d8,  =0
		VLDR.F64 d9,  =0
		VLDR.F64 d10, =0
		VLDR.F64 d11, =0
		VLDR.F64 d12, =0
		VLDR.F64 d13, =0
		VLDR.F64 d14, =0
		VLDR.F64 d15, =0

;/***************************************************************************************************/
;/*																									*/
;/*		initialize TCM & RAM																		*/
;/*																									*/
;/***************************************************************************************************/
;/*-------------------------------------------------------------------------------------------------*/
;/* B0TCM DATA copy																					*/
;/*-------------------------------------------------------------------------------------------------*/
;	ldr		r0, =|Load$$B0TCM$$Base|				;/* Runtime Load Base = 0xFFF28000 */
;	ldr		r1, =|Image$$B0TCM$$Base|						;/* B0TCM Fill Base = 0x67FA0000 */
;	ldr		r2, =|Image$$B0TCM$$Limit|						;/* B0TCM RO-DATA Length */
;;	ldr		r3, =|Image$$B0TCM$$RW$$Length|					;/* B0TCM RW-DATA Length */
;;	ADD		r2, r2, r3										;/* B0TCM Copy Length */
;B0TCMCopyLoop
;	LDR		r3, [r0], #4									;/* R3 = Loaded AXI Code */
;	STR		r3, [r1], #4									;/* R3 = Stored AXI Code */
;	CMP		r1, r2
;	BLT		B0TCMCopyLoop								;/* Loop until the end address of reached	*/
;/*-------------------------------------------------------------------------------------------------*/
;/* B0TCM ZI region 0clear																			*/
;/*-------------------------------------------------------------------------------------------------*/
;;	LDR		r2, =|Image$$B0TCM$$ZI$$Limit|					;/* B0TCM end address */
;	LDR		r2, =B0TCM_LOAD_END					;/* B0TCM end address */
;	MOV		r3, #0											;/* 0 fill */
;B0TCMClearLoop
;	STR		r3, [r1], #4									;/* R3 = Stored AXI Code */
;	CMP		r1, r2
;	BLT		B0TCMClearLoop								;/* Loop until the end address of reached	*/
;/*-------------------------------------------------------------------------------------------------*/
;/* B1TCM CODE copy																					*/
;/*-------------------------------------------------------------------------------------------------*/
;	ldr		r0, =|Load$$B1TCM$$Base|				;/* Runtime Load Base = 0xFFF28000 */
;	ldr		r1, =|Image$$B1TCM$$Base|						;/* B0TCM Fill Base = 0x67FA0000 */
;	ldr		r2, =|Image$$B1TCM$$Limit|						;/* B0TCM RO-DATA Length */
;B1TCMCopyLoop
;	LDR		r3, [r0], #4									;/* R3 = Loaded AXI Code */
;	STR		r3, [r1], #4									;/* R3 = Stored AXI Code */
;	CMP		r1, r2
;	BLT		B1TCMCopyLoop								;/* Loop until the end address of reached	*/
;/*-------------------------------------------------------------------------------------------------*/
;/* ATCM CODE copy																					*/
;/*-------------------------------------------------------------------------------------------------*/
;	ldr		r0, =|Load$$ATCM$$Base|				;/* Runtime Load Base = 0xFFF28000 */
;	ldr		r1, =|Image$$ATCM$$Base|						;/* B0TCM Fill Base = 0x67FA0000 */
;	ldr		r2, =|Image$$ATCM$$Limit|						;/* B0TCM RO-DATA Length */
;ATCMCopyLoop
;	LDR		r3, [r0]									;/* R3 = Loaded ATCM Code from Flash <S062> */
;	STR		r3, [r1]									;/* R3 = Stored ATCM Code <S062> */
;	LDR		r4, [r1]									;/* R4 = Loaded ATCM Code from ATCM <S062> */
;	CMP		r3, r4										;/* verify ATCM write data <S062> */
;	ADDNE	r5, #1										;/* verify error count <S062> */
;	BNE		ATCMCopyLoop								;/* verify write data <S062>	*/
;	ADD		r0, #4										;/* <S062> */
;	ADD		r1, #4										;/* <S062> */
;	CMP		r1, r2
;	BLT		ATCMCopyLoop								;/* Loop until the end address of reached	*/
;/*-------------------------------------------------------------------------------------------------*/
;/* AXIRAM CODE copy																					*/
;/*-------------------------------------------------------------------------------------------------*/
;	ldr		r0, =|Load$$AXIRAM_CODE$$Base|				;/* Runtime Load Base = 0xFFF28000 */
;	ldr		r1, =|Image$$AXIRAM_CODE$$Base|						;/* B0TCM Fill Base = 0x67FA0000 */
;	ldr		r2, =|Image$$AXIRAM_CODE$$Limit|						;/* B0TCM RO-DATA Length */
;AXIRAM_CODECopyLoop
;	LDR		r3, [r0], #4									;/* R3 = Loaded AXI Code */
;	STR		r3, [r1], #4									;/* R3 = Stored AXI Code */
;	CMP		r1, r2
;	BLT		AXIRAM_CODECopyLoop								;/* Loop until the end address of reached	*/
;/*-------------------------------------------------------------------------------------------------*/
;/* B0TCM DATA copy																					*/
;/*-------------------------------------------------------------------------------------------------*/
;	ldr		r0, =|Load$$AXIRAM_DATA$$Base|				;/* Runtime Load Base = 0xFFF28000 */
;	ldr		r1, =|Image$$AXIRAM_DATA$$Base|						;/* B0TCM Fill Base = 0x67FA0000 */
;	ldr		r2, =|Image$$AXIRAM_DATA$$Limit|						;/* B0TCM RO-DATA Length */
;AXIRAM_DATACopyLoop
;	LDR		r3, [r0], #4									;/* R3 = Loaded AXI Code */
;	STR		r3, [r1], #4									;/* R3 = Stored AXI Code */
;	CMP		r1, r2
;	BLT		AXIRAM_DATACopyLoop								;/* Loop until the end address of reached	*/
;/*-------------------------------------------------------------------------------------------------*/
;/* B0TCM ZI region 0clear																			*/
;/*-------------------------------------------------------------------------------------------------*/
;;	LDR		r2, =|Image$$B0TCM$$ZI$$Limit|					;/* B0TCM end address */
;	LDR		r2, =AXIRAM_DATA_LOAD_END					;/* B0TCM end address */
;	MOV		r3, #0											;/* 0 fill */
;AXIRAM_DATAClearLoop
;	STR		r3, [r1], #4									;/* R3 = Stored AXI Code */
;	CMP		r1, r2
;	BLT		AXIRAM_DATAClearLoop								;/* Loop until the end address of reached	*/


;;------ initialize CACHE ------


;; -----------------------------------------------------------------------------
;; その他のサブシステム, IPの初期化
;; -----------------------------------------------------------------------------
			;;------ decode runtime ---------

;<S146> >>>>>
;; -----------------------------------------------------------------------------
;; 展開用コードとテーブルをRAMにコピー
;; -----------------------------------------------------------------------------
			ldr		r0, =|Load$$ELFLOAD_CODE$$Base|		;/* FLASH上ベース	*/
			ldr		r1, =|Image$$ELFLOAD_CODE$$Base|	;/* RAM上ベース		*/
			ldr		r2, =|Image$$ELFLOAD_CODE$$Limit|	;/* リミット値		*/
ELFLOAD_CODECopyLoop
			LDR		r3, [r0], #4						;/* R3 = Loaded AXI Code */
			STR		r3, [r1], #4						;/* R3 = Stored AXI Code */
			CMP		r1, r2
			BLT		ELFLOAD_CODECopyLoop				;/* Loop until the end address of reached	*/

;; -----------------------------------------------------------------------------
;; ランタイムファームウェア展開
;; -----------------------------------------------------------------------------
			NOP
			BL		EFW_Loader
			NOP

;; -----------------------------------------------------------------------------
;; 変更していたリージョンを本来の設定(フェッチ無効)にする
;; -----------------------------------------------------------------------------
			;; -----------------------------------------------------------------
			;; Region No.6 : AXIRAM_DATA (<----CPU)
			;; -----------------------------------------------------------------
			LDR r4,=0x00000006  ;; MPU Memory Region Number Register
			LDR r7,=0x0000130c  ;; MPU Region Access Control Register

			MCR     p15, 0, r4, c6, c2, 0   ;; Write MPU Memory Region Number Register
			MCR     p15, 0, r7, c6, c1, 4   ;; Write MPU Region Access Control Register

;; -----------------------------------------------------------------------------
;; 展開用コードとテーブルをクリア
;; -----------------------------------------------------------------------------
;			ldr		r1, =|Image$$ELFLOAD_DATA$$Base|	;/* RAM上ベース		*/
;			ldr		r2, =|Image$$ELFLOAD_DATA$$Limit|	;/* リミット値		*/
;			MOV		r3, #0								;/* 0 fill */
;ELFLOAD_DATAClearLoop
;			STR		r3, [r1], #4						;/* R3 = Stored AXI Code */
;			CMP		r1, r2
;			BLT		ELFLOAD_DATAClearLoop				;/* Loop until the end address of reached	*/
;
;			ldr		r1, =|Image$$ELFLOAD_CODE$$Base|	;/* RAM上ベース		*/
;			ldr		r2, =|Image$$ELFLOAD_CODE$$Limit|	;/* リミット値		*/
;			MOV		r3, #0								;/* 0 fill */
;ELFLOAD_CODEClearLoop
;			STR		r3, [r1], #4						;/* R3 = Stored AXI Code */
;			CMP		r1, r2
;			BLT		ELFLOAD_CODEClearLoop				;/* Loop until the end address of reached	*/
;<<<<< <S146>

;; -----------------------------------------------------------------------------
;; mainに分岐
;; -----------------------------------------------------------------------------
			NOP
			B		MercuryMain
			NOP

end
			NOP
			NOP
			NOP 
			NOP
			B   end
			NOP
Undefined_Handler
			B	Undefined_Handler
SWI_Handler
			B	SWI_Handler
Prefetch_Handler
			B	Prefetch_Handler
Abort_Handler
			B	Abort_Handler
FIQ_Handler
			B	FIQ_Handler
			NOP
			NOP


		END
;/*-------end of file----------------------------------------------------------------------------------------*/
