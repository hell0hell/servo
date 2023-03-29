################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Apl/CurManuAdj/FunCurManuAdj.c 

OBJS += \
./00_src/01_Standard/Kernel/Round/Apl/CurManuAdj/FunCurManuAdj.o 

C_DEPS += \
./00_src/01_Standard/Kernel/Round/Apl/CurManuAdj/FunCurManuAdj.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Round/Apl/CurManuAdj/FunCurManuAdj.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Apl/CurManuAdj/FunCurManuAdj.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ƒRƒ“ƒpƒCƒ‰'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


