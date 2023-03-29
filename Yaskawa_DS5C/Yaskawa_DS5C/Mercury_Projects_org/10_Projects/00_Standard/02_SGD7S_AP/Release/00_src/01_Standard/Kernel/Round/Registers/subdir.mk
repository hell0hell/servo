################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.c 

OBJS += \
./00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.o \
./00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.o 

C_DEPS += \
./00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.d \
./00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


