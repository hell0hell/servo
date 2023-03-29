################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Boot/Config/IniPrmCalc.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Boot/Config/MercuryGlobals.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Boot/Config/SystemConfigulation.c 

OBJS += \
./00_src/01_Standard/Kernel/Boot/Config/IniPrmCalc.o \
./00_src/01_Standard/Kernel/Boot/Config/MercuryGlobals.o \
./00_src/01_Standard/Kernel/Boot/Config/SystemConfigulation.o 

C_DEPS += \
./00_src/01_Standard/Kernel/Boot/Config/IniPrmCalc.d \
./00_src/01_Standard/Kernel/Boot/Config/MercuryGlobals.d \
./00_src/01_Standard/Kernel/Boot/Config/SystemConfigulation.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Boot/Config/IniPrmCalc.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Boot/Config/IniPrmCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Boot/Config/MercuryGlobals.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Boot/Config/MercuryGlobals.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Boot/Config/SystemConfigulation.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Boot/Config/SystemConfigulation.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


