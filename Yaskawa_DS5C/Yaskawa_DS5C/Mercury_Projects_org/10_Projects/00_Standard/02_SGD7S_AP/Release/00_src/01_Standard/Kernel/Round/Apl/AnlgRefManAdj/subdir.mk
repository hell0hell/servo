################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgTrqRefManAdj.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgVelRefManAdj.c 

OBJS += \
./00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgTrqRefManAdj.o \
./00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgVelRefManAdj.o 

C_DEPS += \
./00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgTrqRefManAdj.d \
./00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgVelRefManAdj.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgTrqRefManAdj.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgTrqRefManAdj.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgVelRefManAdj.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgVelRefManAdj.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


