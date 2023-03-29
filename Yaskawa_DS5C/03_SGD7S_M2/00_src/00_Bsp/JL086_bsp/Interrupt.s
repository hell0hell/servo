;/****************************************************************************************************/
;/*																									*/
;/*																									*/
;/*		init_SGD7.s : MercurySoC�u�[�g�v���O����													*/
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
;/*		Rev.1.00 : 2013.07.13  Y.Tanaka		����													*/
;/*																									*/
;/****************************************************************************************************/
	CODE32
	REQUIRE8
	PRESERVE8

;/****************************************************************************************************/
;/*																									*/
;/*		�萔��`																					*/
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
;/*		�֘A��`�iROM->RAM�W�J�p�j																	*/
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
;/*		���荞�ݓ��͎�����																			*/
;/*																									*/
;/***************************************************************************************************/
;/*-------------------------------------------------------------------------------------------------*/
;/* �֐��C���|�[�g																					*/
;/*-------------------------------------------------------------------------------------------------*/
	IMPORT	Common_IRQHandler0
	IMPORT	Common_IRQHandler1
	IMPORT	ScanAIntHandler
	IMPORT	ScanBIntHandler
	IMPORT	ScanCIntHandler
;/*-------------------------------------------------------------------------------------------------*/
;/* �֐��G�N�X�|�[�g																				*/
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
;/* VIC VEbit�Z�b�g																					*/
;/*-------------------------------------------------------------------------------------------------*/
;/* CP15 �̐ݒ�ɂ��AVIC �C���^�[�t�F�[�X�ݒ��L���ɂ���											*/
;/* (�V�X�e�����䃌�W�X�^(c1�@24bit)VE�@bit���Z�b�g)												*/
__SET_VE	FUNCTION
		PUSH	{r0,lr}
	    MRC p15, 0, r0, c1, c0, 0
	    ORR r0, r0, #(0x1 << 24)
	    MCR p15, 0, r0, c1, c0, 0
		POP	{r0,pc}
	ENDFUNC

	AREA VECT_CODE, CODE, READONLY
;/*-------------------------------------------------------------------------------------------------*/
;/* ���W�X�^�ޔ����v���n���h���W�����v for ScanA													*/
;/*-------------------------------------------------------------------------------------------------*/
ScanAHandler
;����m�FIRQ���W�X�^�ޔ�����
		PUSH    {r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq��ޔ�
		VPUSH   {d0-d15}				;���荞�ݑO��FPU���W�X�^��ޔ�	<S151> VFP(FPU)���W�X�^�����ǉ�

		MRS		r12, SPSR				;SPSR_irq��r12�ɃR�s�[
		PUSH	{r12}					;r12���v�b�V��
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYS���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;�������烆�[�U���[�h�̃��W�X�^�B
		AND		R1, r14, #4				;�X�^�b�N�̐�����e�X							<S0E3>
		SUB		sp, sp, R1				;�X�^�b�N����									<S0E3>
		PUSH	{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr��ޔ�		<S0E3>
		BL		ScanAIntHandler			;ScanA�n���h���փW�����v
		POP		{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr�𕜋A		<S0E3>
		ADD		sp, sp, R1				;�X�^�b�N�����𕜌�								<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQ���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;��������IRQ���[�h�̃��W�X�^�B
		POP		{r12};	MSR				SPSR_cf, r12	;SPSR_irq�𕜋A					<S0E3>
		MSR		SPSR_cxsf, r12			;SPSR_irq�𕜋A	<Ando>

		VPOP   {d0-d15}					;���荞�ݑO��FPU���W�X�^�𕜋A	<S151> VFP(FPU)���W�X�^�����ǉ�
		POP		{r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq�̕��A
		SUBS	pc, lr, #4				;IRQ����̕��A�A�h���X�v�Z


;/*-------------------------------------------------------------------------------------------------*/
;/* ���W�X�^�ޔ����v���n���h���W�����v for ScanB													*/
;/*-------------------------------------------------------------------------------------------------*/
ScanBHandler
;����m�FIRQ���W�X�^�ޔ�����
		PUSH    {r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq��ޔ�
		VPUSH   {d0-d15}				;���荞�ݑO��FPU���W�X�^��ޔ�	<S151> VFP(FPU)���W�X�^�����ǉ�

		MRS		r12, SPSR				;SPSR_irq��r12�ɃR�s�[
		PUSH	{r12}					;r12���v�b�V��
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYS���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;�������烆�[�U���[�h�̃��W�X�^�B
		AND		R1, r14, #4				;�X�^�b�N�̐�����e�X�g							<S0E3>
		SUB		sp, sp, R1				;�X�^�b�N����									<S0E3>
		PUSH	{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr��ޔ�		<S0E3>
		BL		ScanBIntHandler			;ScanB�n���h���փW�����v
		POP		{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr�𕜋A		<S0E3>
		ADD		sp, sp, R1				;�X�^�b�N�����𕜌�								<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQ���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;��������IRQ���[�h�̃��W�X�^�B
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irq�𕜋A	<Ando>
		MSR		SPSR_cxsf, r12			;SPSR_irq�𕜋A	<Ando>

		VPOP   {d0-d15}					;���荞�ݑO��FPU���W�X�^�𕜋A	<S151> VFP(FPU)���W�X�^�����ǉ�
		POP		{r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq�̕��A
		SUBS	pc, lr, #4				;IRQ����̕��A�A�h���X�v�Z


;/*-------------------------------------------------------------------------------------------------*/
;/* ���W�X�^�ޔ����v���n���h���W�����v for ScanC													*/
;/*-------------------------------------------------------------------------------------------------*/
ScanCHandler
;����m�FIRQ���W�X�^�ޔ�����
		PUSH    {r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq��ޔ�
		VPUSH   {d0-d15}				;���荞�ݑO��FPU���W�X�^��ޔ�	<S151> VFP(FPU)���W�X�^�����ǉ�

		MRS		r12, SPSR				;SPSR_irq��r12�ɃR�s�[
		PUSH	{r12}					;r12���v�b�V��
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYS���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;�������烆�[�U���[�h�̃��W�X�^�B
		AND		R1, r14, #4				;�X�^�b�N�̐�����e�X�g							<S0E3>
		SUB		sp, sp, R1				;�X�^�b�N����									<S0E3>
		PUSH		{R1, lr}			;�X�^�b�N�����l�A���荞�ݑO��lr_usr��ޔ�		<S0E3>
		BL		ScanCIntHandler			;ScanC�n���h���փW�����v
		POP		{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr�𕜋A		<S0E3>
		ADD		sp, sp, R1				;�X�^�b�N�����𕜌�								<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQ���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;��������IRQ���[�h�̃��W�X�^�B
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irq�𕜋A	<Ando>
		MSR		SPSR_cxsf, r12			;SPSR_irq�𕜋A	<Ando>

		VPOP   {d0-d15}					;���荞�ݑO��FPU���W�X�^�𕜋A	<S151> VFP(FPU)���W�X�^�����ǉ�
		POP		{r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq�̕��A
		SUBS	pc, lr, #4				;IRQ����̕��A�A�h���X�v�Z


;/*-------------------------------------------------------------------------------------------------*/
;/* ���W�X�^�ޔ����v���n���h���W�����v for ARMPF_VIC_ISC0											*/
;/*-------------------------------------------------------------------------------------------------*/
IRQHandler0
;����m�FIRQ���W�X�^�ޔ�����
		PUSH    {r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq��ޔ�
		VPUSH   {d0-d15}				;���荞�ݑO��FPU���W�X�^��ޔ�	<S151> VFP(FPU)���W�X�^�����ǉ�

		MRS		r12, SPSR					;SPSR_irq��r12�ɃR�s�[
		PUSH	{r12}					;r12���v�b�V��
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYS���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;�������烆�[�U���[�h�̃��W�X�^�B
		AND		R1, r14, #4				;�X�^�b�N�̐�����e�X�g						<S0E3>
		SUB		sp, sp, R1				;�X�^�b�N����								<S0E3>
		PUSH	{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr��ޔ�	<S0E3>
		BL		Common_IRQHandler0		;IRQ�n���h���փW�����v
		POP		{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr�𕜋A	<S0E3>
		ADD		sp, sp, R1				;�X�^�b�N�����𕜌�							<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQ���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;��������IRQ���[�h�̃��W�X�^�B
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irq�𕜋A								<S0E3>
		MSR		SPSR_cxsf, r12			;SPSR_irq�𕜋A								<S0E3>

		VPOP   {d0-d15}					;���荞�ݑO��FPU���W�X�^�𕜋A	<S151> VFP(FPU)���W�X�^�����ǉ�
		POP		{r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq�̕��A
		SUBS			pc, lr, #4		;IRQ����̕��A�A�h���X�v�Z


;/*-------------------------------------------------------------------------------------------------*/
;/* ���W�X�^�ޔ����v���n���h���W�����v for ARMPF_VIC_ISC1											*/
;/*-------------------------------------------------------------------------------------------------*/
IRQHandler1
;����m�FIRQ���W�X�^�ޔ�����
		PUSH    {r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq��ޔ�
		VPUSH   {d0-d15}				;���荞�ݑO��FPU���W�X�^��ޔ�	<S151> VFP(FPU)���W�X�^�����ǉ�

		MRS		r12, SPSR				;SPSR_irq��r12�ɃR�s�[
		PUSH	{r12}					;r12���v�b�V��
		MSR		CPSR_c, #MODE_SYS :OR: IRQ_DISABLE
										;SYS���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;�������烆�[�U���[�h�̃��W�X�^�B
		AND		R1, r14, #4				;�X�^�b�N�̐�����e�X�g						<S0E3>
		SUB		sp, sp, R1				;�X�^�b�N����								<S0E3>
		PUSH	{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr��ޔ�	<S0E3>
		BL		Common_IRQHandler1		;IRQ�n���h���փW�����v
		POP		{R1, lr}				;�X�^�b�N�����l�A���荞�ݑO��lr_usr�𕜋A	<S0E3>
		ADD		sp, sp, R1				;�X�^�b�N�����𕜌�							<S0E3>
		MSR		CPSR_c, #MODE_IRQ :OR: IRQ_DISABLE
										;IRQ���[�h�ɐ؂�ւ��BIRQ�f�B�Z�[�u��
										;��������IRQ���[�h�̃��W�X�^�B
		POP		{r12}
;		MSR		SPSR_cf, r12			;SPSR_irq�𕜋A
		MSR		SPSR_cxsf, r12			;SPSR_irq�𕜋A

		VPOP   {d0-d15}					;���荞�ݑO��FPU���W�X�^�𕜋A	<S151> VFP(FPU)���W�X�^�����ǉ�
		POP		{r0-r12,lr}				;���荞�ݑO�̃��W�X�^,lr_irq�̕��A
		SUBS	pc, lr, #4				;IRQ����̕��A�A�h���X�v�Z


;/*-------------------------------------------------------------------------------------------------*/
;/* ���荞�݃C�l�[�u��																				*/
;/*-------------------------------------------------------------------------------------------------*/
__EI	FUNCTION
		PUSH	{r0,lr}
		MRS		r0, CPSR;		/* st <- CPSR */
		BIC		r0, r0, #0x80;		/* clear interrupt enable bit */
		MSR		CPSR_c, r0;		/* CPSR <- st */
		POP		{r0,PC}
	ENDFUNC


;/*-------------------------------------------------------------------------------------------------*/
;/* ���荞�݃f�B�Z�[�u��																				*/
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
